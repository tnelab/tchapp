using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.IO;

namespace TchApp.Razor
{
    public class ViewContext
    {
        private DynamicViewData _viewBag;
        public ViewContext()
        {
            ViewData = new Dictionary<string, object>();
        }
        public ViewContext(
            ViewContext viewContext,
            IView view,
            Dictionary<string,object> viewData,
            TextWriter writer)
        {
            if (viewContext == null)
            {
                throw new ArgumentNullException(nameof(viewContext));
            }

            if (view == null)
            {
                throw new ArgumentNullException(nameof(view));
            }

            if (viewData == null)
            {
                throw new ArgumentNullException(nameof(viewData));
            }

            if (writer == null)
            {
                throw new ArgumentNullException(nameof(writer));
            }


            ExecutingFilePath = viewContext.ExecutingFilePath;
            View = view;
            ViewData = viewData;
            Writer = writer;
        }

        public Dictionary<string,object> ViewData { get; set; }
        public TextWriter Writer { get; set; }
        public IView View { get; set; }
        public string ExecutingFilePath { get; set; }
        public dynamic ViewBag
        {
            get
            {
                if (_viewBag == null)
                {
                    _viewBag = new DynamicViewData(() => ViewData);
                }

                return _viewBag;
            }
        }
    }
}
