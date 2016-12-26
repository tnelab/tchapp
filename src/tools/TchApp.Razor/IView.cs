using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace TchApp.Razor
{
    public interface IView
    {
        /// <summary>
        /// Gets the path of the view as resolved by the <see cref="IViewEngine"/>.
        /// </summary>
        string Path { get; }

        /// <summary>
        /// Asynchronously renders the view using the specified <paramref name="context"/>.
        /// </summary>
        /// <param name="context">The <see cref="ViewContext"/>.</param>
        /// <returns>A <see cref="Task"/> that on completion renders the view.</returns>
        Task RenderAsync(ViewContext context);
    }
}
