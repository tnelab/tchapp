using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Text;
using System.IO;
using System.Reflection;

namespace TchApp.Razor
{
    public class TchRazorView : IView
    {
        
        public TchRazorView(
            IReadOnlyList<IRazorPage> viewStartPages,
            IRazorPage razorPage)
        {
            if (viewStartPages == null)
            {
                throw new ArgumentNullException(nameof(viewStartPages));
            }

            if (razorPage == null)
            {
                throw new ArgumentNullException(nameof(razorPage));
            }

            ViewStartPages = viewStartPages;
            RazorPage = razorPage;
        }

        /// <inheritdoc />
        public string Path
        {
            get { return RazorPage.Path; }
        }

        /// <summary>
        /// Gets <see cref="IRazorPage"/> instance that the views executes on.
        /// </summary>
        public IRazorPage RazorPage { get; }

        /// <summary>
        /// Gets the sequence of _ViewStart <see cref="IRazorPage"/> instances that are executed by this view.
        /// </summary>
        public IReadOnlyList<IRazorPage> ViewStartPages { get; }

        /// <inheritdoc />
        public virtual async Task RenderAsync(ViewContext context)
        {
            if (context == null)
            {
                throw new ArgumentNullException(nameof(context));
            }

            // This GetRequiredService call is by design. ViewBufferScope is a scoped service, RazorViewEngine
            // is the component responsible for creating RazorViews and it is a Singleton service. It doesn't
            // have access to the RequestServices so requiring the service when we render the page is the best
            // we can do.
            var bodyWriter = await RenderPageAsync(RazorPage, context, invokeViewStarts: true);
            await RenderLayoutAsync(context, bodyWriter);
        }

        private async Task<TextWriter> RenderPageAsync(
            IRazorPage page,
            ViewContext context,
            bool invokeViewStarts)
        {
            var writer = context.Writer as StringWriter;
            if (writer == null)
            {
                writer = new StringWriter();
            }
            // The writer for the body is passed through the ViewContext, allowing things like HtmlHelpers
            // and ViewComponents to reference it.
            var oldWriter = context.Writer;
            var oldFilePath = context.ExecutingFilePath;

            context.Writer = writer;
            context.ExecutingFilePath = page.Path;

            try
            {
                if (invokeViewStarts)
                {
                    // Execute view starts using the same context + writer as the page to render.
                    await RenderViewStartsAsync(context);
                }

                await RenderPageCoreAsync(page, context);
                return writer;              
            }
            catch(Exception ex)
            {
                throw ex;
            }
            finally
            {
                context.Writer = oldWriter;
                context.ExecutingFilePath = oldFilePath;
            }
        }

        private Task RenderPageCoreAsync(IRazorPage page, ViewContext context)
        {
            page.ViewContext = context;
            return page.ExecuteAsync();
        }

        private async Task RenderViewStartsAsync(ViewContext context)
        {
            string layout = null;
            var oldFilePath = context.ExecutingFilePath;
            try
            {
                for (var i = 0; i < ViewStartPages.Count; i++)
                {
                    var viewStart = ViewStartPages[i];
                    context.ExecutingFilePath = viewStart.Path;

                    // If non-null, copy the layout value from the previous view start to the current. Otherwise leave
                    // Layout default alone.
                    if (layout != null)
                    {
                        viewStart.Layout = layout;
                    }

                    await RenderPageCoreAsync(viewStart, context);

                    // Pass correct absolute path to next layout or the entry page if this view start set Layout to a
                    // relative path.
                    layout = viewStart.Layout;
                }
            }
            finally
            {
                context.ExecutingFilePath = oldFilePath;
            }

            // If non-null, copy the layout value from the view start page(s) to the entry page.
            if (layout != null)
            {
                RazorPage.Layout = layout;
            }
        }

        private async Task RenderLayoutAsync(
            ViewContext context,
            TextWriter bodyWriter)
        {
            // A layout page can specify another layout page. We'll need to continue
            // looking for layout pages until they're no longer specified.
            var previousPage = RazorPage;
            var renderedLayouts = new List<IRazorPage>();

            // This loop will execute Layout pages from the inside to the outside. With each
            // iteration, bodyWriter is replaced with the aggregate of all the "body" content
            // (including the layout page we just rendered).
            while (!string.IsNullOrEmpty(previousPage.Layout))
            {
                var layoutPage = GetLayoutPage(context, previousPage.Path, previousPage.Layout);

                if (renderedLayouts.Count > 0 &&
                    renderedLayouts.Any(l => string.Equals(l.Path, layoutPage.Path, StringComparison.Ordinal)))
                {
                    // If the layout has been previously rendered as part of this view, we're potentially in a layout
                    // rendering cycle.
                    throw new InvalidOperationException();
                }

                // Notify the previous page that any writes that are performed on it are part of sections being written
                // in the layout.
                previousPage.IsLayoutBeingRendered = true;
                layoutPage.PreviousSectionWriters = previousPage.SectionWriters;
                layoutPage.BodyContent = (bodyWriter as StringWriter).GetStringBuilder().ToString();
                bodyWriter = await RenderPageAsync(layoutPage, context, invokeViewStarts: false);

                renderedLayouts.Add(layoutPage);
                previousPage = layoutPage;
            }

            // Now we've reached and rendered the outer-most layout page. Nothing left to execute.

            // Ensure all defined sections were rendered or RenderBody was invoked for page without defined sections.
            foreach (var layoutPage in renderedLayouts)
            {
                layoutPage.EnsureRenderedBodyOrSections();
            }
            if(context.Writer as StringWriter == null)
            {
                context.Writer = bodyWriter;
            }
            //if (bodyWriter.IsBuffering)
            //{
            //    // If IsBuffering - then we've got a bunch of content in the view buffer. How to best deal with it
            //    // really depends on whether or not we're writing directly to the output or if we're writing to
            //    // another buffer.
            //    var viewBufferTextWriter = context.Writer as ViewBufferTextWriter;
            //    if (viewBufferTextWriter == null || !viewBufferTextWriter.IsBuffering)
            //    {
            //        // This means we're writing to a 'real' writer, probably to the actual output stream.
            //        // We're using PagedBufferedTextWriter here to 'smooth' synchronous writes of IHtmlContent values.
            //        using (var writer = _bufferScope.CreateWriter(context.Writer))
            //        {
            //            await bodyWriter.Buffer.WriteToAsync(writer, _htmlEncoder);
            //        }
            //    }
            //    else
            //    {
            //        // This means we're writing to another buffer. Use MoveTo to combine them.
            //        bodyWriter.Buffer.MoveTo(viewBufferTextWriter.Buffer);
            //        return;
            //    }
            //}
        }

        private IRazorPage GetLayoutPage(ViewContext context, string executingFilePath, string layoutPath)
        {
            IRazorPage layoutPage = TchRazorActivator.GetPage(context, layoutPath);
            return layoutPage;
        }
    }
}
