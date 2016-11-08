namespace tchclient.ui
{
    using System.Threading.Tasks;

    public class test : BasePage
    {
        #line hidden
        public test()
        {
        }

        #pragma warning disable 1998
        public override async Task ExecuteAsync()
        {
            WriteLiteral("<html>\r\n<head>\r\n    <title>cshtml test</title>\r\n</head>\r\n<body>\r\n    <h1>hi cshtml</h1>\r\n");
#line 7 "test.cshtml"
    

#line default
#line hidden

#line 7 "test.cshtml"
      		
        for (int i = 0; i < 26; ++i)
        {

#line default
#line hidden

            WriteLiteral("            <h1>hi ");
#line 10 "test.cshtml"
              Write(i.ToString());

#line default
#line hidden
            WriteLiteral("</h1>\r\n");
#line 11 "test.cshtml"

        }
    

#line default
#line hidden

            WriteLiteral("</body>\r\n</html>");
        }
        #pragma warning restore 1998
    }
}

namespace tchclient.ui{
	using System.Text;
	using System.Threading.Tasks;
	public abstract class BasePage{
		StringBuilder htmlBuilder=new StringBuilder();
		public override string ToString(){
			return htmlBuilder.ToString();
		}
		public abstract Task ExecuteAsync();
		public void WriteLiteral(string value){
			htmlBuilder.Append(value);
		}
		public void Write(string value){
			htmlBuilder.Append(value);
		}
	}
}