using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace RazorCompiler
{
    public class Program
    {
        public static void Main(string[] args)
        {
            RazorCompiler compler = new RazorCompiler();
            compler.Compile(args[0]);
        }
    }
}
