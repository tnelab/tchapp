using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Dynamic;
using System.IO;

namespace TchApp.Razor
{
    public abstract class TchRazorPage:IRazorPage
    {
        private readonly HashSet<string> _renderedSections = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        private bool _renderedBody;
        private bool _ignoreBody;
        private HashSet<string> _ignoredSections;

        public TchRazorPage()
        {
            SectionWriters = new Dictionary<string, RenderAsyncDelegate>(StringComparer.OrdinalIgnoreCase);
        }

        public string Path { get; set; }

        public ViewContext ViewContext { get; set; }
        
        public string Layout { get; set; }

        public virtual TextWriter Output
        {
            get
            {
                if (ViewContext == null)
                {
                    var message = "ViewContext is null at TchRazorPage.Output";
                    throw new InvalidOperationException(message);
                }

                return ViewContext.Writer;
            }
        }

        public dynamic ViewBag => ViewContext?.ViewBag;

        public string BodyContent { get; set; }

        public bool IsLayoutBeingRendered { get; set; }

        /// <inheritdoc />
        public IDictionary<string, RenderAsyncDelegate> PreviousSectionWriters { get; set; }
        
        public IDictionary<string, RenderAsyncDelegate> SectionWriters { get; }

        public abstract Task ExecuteAsync();        


        public virtual void Write(string value)
        {
            Output.Write(value);
        }

        public virtual void WriteTo(TextWriter writer, object value)
        {
            if (writer == null)
            {
                throw new ArgumentNullException(nameof(writer));
            }
            if (value != null)
            {
                WriteTo(writer, value.ToString());
            }
        }
        public virtual void WriteTo(TextWriter writer, string value)
        {
            if (writer == null)
            {
                throw new ArgumentNullException(nameof(writer));
            }
            if (!string.IsNullOrEmpty(value))
            {
                writer.Write(value);
            }
        }
        public virtual void WriteLiteral(string value)
        {
            if(!string.IsNullOrEmpty(value))
                Output.Write(value);
        }

        public virtual void WriteLiteralTo(TextWriter writer, object value)
        {
            if (writer == null)
            {
                throw new ArgumentNullException(nameof(writer));
            }

            if (value != null)
            {
                WriteLiteralTo(writer, value.ToString());
            }
        }

        public virtual void WriteLiteralTo(TextWriter writer, string value)
        {
            if (writer == null)
            {
                throw new ArgumentNullException(nameof(writer));
            }

            if (!string.IsNullOrEmpty(value))
            {
                writer.Write(value);
            }
        }

        protected virtual string RenderBody()
        {
            if (BodyContent == null)
            {
                throw new InvalidOperationException();
            }

            _renderedBody = true;
            return BodyContent;
        }

        public void IgnoreBody()
        {
            _ignoreBody = true;
        }

        public void DefineSection(string name, RenderAsyncDelegate section)
        {
            if (name == null)
            {
                throw new ArgumentNullException(nameof(name));
            }

            if (section == null)
            {
                throw new ArgumentNullException(nameof(section));
            }

            if (SectionWriters.ContainsKey(name))
            {
                throw new InvalidOperationException();
            }
            SectionWriters[name] = section;
        }

        public bool IsSectionDefined(string name)
        {
            if (name == null)
            {
                throw new ArgumentNullException(nameof(name));
            }

            EnsureMethodCanBeInvoked(nameof(IsSectionDefined));
            return PreviousSectionWriters.ContainsKey(name);
        }

        public string RenderSection(string name)
        {
            if (name == null)
            {
                throw new ArgumentNullException(nameof(name));
            }

            return RenderSection(name, required: true);
        }

        public string RenderSection(string name, bool required)
        {
            if (name == null)
            {
                throw new ArgumentNullException(nameof(name));
            }

            EnsureMethodCanBeInvoked(nameof(RenderSection));

            var task = RenderSectionAsyncCore(name, required);
            return task.GetAwaiter().GetResult();
        }

        public Task<string> RenderSectionAsync(string name)
        {
            if (name == null)
            {
                throw new ArgumentNullException(nameof(name));
            }

            return RenderSectionAsync(name, required: true);
        }

        public Task<string> RenderSectionAsync(string name, bool required)
        {
            if (name == null)
            {
                throw new ArgumentNullException(nameof(name));
            }

            EnsureMethodCanBeInvoked(nameof(RenderSectionAsync));
            return RenderSectionAsyncCore(name, required);
        }

        private async Task<string> RenderSectionAsyncCore(string sectionName, bool required)
        {
            if (_renderedSections.Contains(sectionName))
            {
                throw new InvalidOperationException();
            }

            RenderAsyncDelegate renderDelegate;
            if (PreviousSectionWriters.TryGetValue(sectionName, out renderDelegate))
            {
                _renderedSections.Add(sectionName);
                await renderDelegate(Output);

                // Return a token value that allows the Write call that wraps the RenderSection \ RenderSectionAsync
                // to succeed.
                return string.Empty;
            }
            else if (required)
            {
                // If the section is not found, and it is not optional, throw an error.
                throw new InvalidOperationException();
            }
            else
            {
                // If the section is optional and not found, then don't do anything.
                return null;
            }
        }

        public void IgnoreSection(string sectionName)
        {
            if (sectionName == null)
            {
                throw new ArgumentNullException(nameof(sectionName));
            }

            if (!PreviousSectionWriters.ContainsKey(sectionName))
            {
                // If the section is not defined, throw an error.
                throw new InvalidOperationException();
            }

            if (_ignoredSections == null)
            {
                _ignoredSections = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            }

            _ignoredSections.Add(sectionName);
        }

        public async Task<string> FlushAsync()
        {
            // Calls to Flush are allowed if the page does not specify a Layout or if it is executing a section in the
            // Layout.
            if (!IsLayoutBeingRendered && !string.IsNullOrEmpty(Layout))
            {
                throw new InvalidOperationException();
            }

            await Output.FlushAsync();
            return string.Empty;
        }

        public void EnsureRenderedBodyOrSections()
        {
            // a) all sections defined for this page are rendered.
            // b) if no sections are defined, then the body is rendered if it's available.
            if (PreviousSectionWriters != null && PreviousSectionWriters.Count > 0)
            {
                var sectionsNotRendered = PreviousSectionWriters.Keys.Except(
                    _renderedSections,
                    StringComparer.OrdinalIgnoreCase);

                string[] sectionsNotIgnored;
                if (_ignoredSections != null)
                {
                    sectionsNotIgnored = sectionsNotRendered.Except(_ignoredSections, StringComparer.OrdinalIgnoreCase).ToArray();
                }
                else
                {
                    sectionsNotIgnored = sectionsNotRendered.ToArray();
                }

                if (sectionsNotIgnored.Length > 0)
                {
                    var sectionNames = string.Join(", ", sectionsNotIgnored);
                    throw new InvalidOperationException();
                }
            }
            else if (BodyContent != null && !_renderedBody && !_ignoreBody)
            {
                // There are no sections defined, but RenderBody was NOT called.
                // If a body was defined and the body not ignored, then RenderBody should have been called.
                throw new InvalidOperationException();
            }
        }                

        private void EnsureMethodCanBeInvoked(string methodName)
        {
            if (PreviousSectionWriters == null)
            {
                throw new InvalidOperationException(methodName);
            }
        }
        public async Task RenderPartialAsync(string partial)
        {
            await TchRazorActivator.GetView(ViewContext,partial).RenderAsync(ViewContext);
        }
    }
}
