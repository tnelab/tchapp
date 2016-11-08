using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CmdTest
{
    enum Gender { 男, 女 }
    class Program
    {
        static void Main(string[] args)
        {

            var t = new { Name = "zmg" };
            string str = $"{t.Name}";
        }
    }
}
