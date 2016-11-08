using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace System
{
    public static class ExceptionExtensions
    {
        public static Exception GetLast(this Exception ex)
        {
            Exception result=ex;
            if (ex.InnerException != null) result = ex.InnerException.GetLast();
            return result;
        }
    }
}
