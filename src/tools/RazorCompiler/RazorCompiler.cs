using System;
using System.Collections.Generic;
using System.IO;
using Microsoft.AspNetCore.Razor;
using Microsoft.AspNetCore.Razor.CodeGenerators;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis;
using System.Reflection;
using System.Runtime.Loader;
using TchApp.Razor;
using System.Text;

namespace RazorCompiler
{
    public class RazorCompiler
    {
       
        public void Compile(string input)
        {
            input =Path.GetFullPath(input);
            if (!Directory.Exists(input))
                throw new Exception($"path {input} not exists");
            DirectoryInfo dir_info = new DirectoryInfo(input);            
            var files = dir_info.GetFiles("*.cshtml", SearchOption.AllDirectories);
            SyntaxTree[] tree = new SyntaxTree[files.Length];
            string code, class_namespace,page_path;
            List<string> page_list = new List<string>();

            for(var i=0;i<files.Length;++i)
            {
                if (!Path.GetFileNameWithoutExtension(files[i].FullName).StartsWith("_"))
                {
                    page_path = files[i].FullName.Replace(input, "").Replace(Path.DirectorySeparatorChar, '/');
                    if (!page_path.StartsWith("/"))
                        page_path = "/" + page_path;
                    page_list.Add(page_path);
                }                
                class_namespace = Path.GetDirectoryName(files[i].FullName).Replace(input, "").Replace(Path.DirectorySeparatorChar, '.');
                if (class_namespace.Length>0&&!class_namespace.StartsWith("."))
                    class_namespace = "." + class_namespace;
                code = getCSharpCode(files[i].FullName,TchRazorActivator.RootNamespace+class_namespace);
                tree[i]=CSharpSyntaxTree.ParseText(code);                                
            }

            var core_dir = Directory.GetParent(typeof(object).GetTypeInfo().Assembly.Location);
            var dlls = core_dir.GetFiles("*.dll", SearchOption.AllDirectories);

            List<MetadataReference> ref_list = new List<MetadataReference>();            
            ref_list.Add(MetadataReference.CreateFromFile(Assembly.GetEntryAssembly().Location));
            ref_list.Add(MetadataReference.CreateFromFile(typeof(IRazorPage).GetTypeInfo().Assembly.Location));
            foreach (var clr_dll in dlls)
            {
                try
                {
                    AssemblyLoadContext.GetAssemblyName(clr_dll.FullName);
                    ref_list.Add(MetadataReference.CreateFromFile(clr_dll.FullName));
                }
                catch (BadImageFormatException) { };
            }

            var option = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary,optimizationLevel:OptimizationLevel.Release);
            CSharpCompilation compilation = CSharpCompilation.Create(Path.GetRandomFileName(),tree,ref_list,option);     
                   
            Assembly compiled_assembly;
            using (var assembly_stream = new MemoryStream())
            {
                var compileResult = compilation.Emit(assembly_stream);
                if (compileResult.Success)
                {
                    assembly_stream.Seek(0, SeekOrigin.Begin);
                    compiled_assembly = AssemblyLoadContext.Default.LoadFromStream(assembly_stream);                    
                }
                else
                {
                    foreach (var error_info in compileResult.Diagnostics)
                    {
                        Console.WriteLine(error_info.ToString());
                    }
                    return;
                }
            }
            TchRazorActivator.InnerAssembly = compiled_assembly;
            foreach (var page in page_list)
            {
                ViewContext view_context = new ViewContext();
                view_context.ExecutingFilePath = page;
                TchRazorActivator.GetView(view_context,page).RenderAsync(view_context);
                var html = (view_context.Writer as StringWriter).GetStringBuilder().ToString();
                var output_path = Path.Combine(input, page.Remove(0, 1).Replace('/', Path.DirectorySeparatorChar));
                output_path=output_path.Remove(output_path.Length - 6, 2);
                File.WriteAllText(output_path,html,Encoding.UTF8);
            }

        }
        string getCSharpCode(string cshtmlFilePath, string rootNamespace)
        {
            var basePath = Path.GetDirectoryName(cshtmlFilePath);
            var fileName = Path.GetFileName(cshtmlFilePath);
            var fileNameNoExtension = Path.GetFileNameWithoutExtension(fileName);
            var codeLang = new CSharpRazorCodeLanguage();
            var host = new RazorEngineHost(codeLang);

            host.DefaultBaseClass = "TchApp.Razor.TchRazorPage";
            host.GeneratedClassContext = new GeneratedClassContext(
                executeMethodName: GeneratedClassContext.DefaultExecuteMethodName,
                writeMethodName: GeneratedClassContext.DefaultWriteMethodName,
                writeLiteralMethodName: GeneratedClassContext.DefaultWriteLiteralMethodName,
                writeToMethodName: "WriteTo",
                writeLiteralToMethodName: "WriteLiteralTo",
                templateTypeName: "HelperResult",
                defineSectionMethodName: "DefineSection",
                generatedTagHelperContext: new GeneratedTagHelperContext());
            var engine = new RazorTemplateEngine(host);

            var cshtmlContent = File.ReadAllText(cshtmlFilePath);

            var generatorResults = engine.GenerateCode(
                    input: new StringReader(cshtmlContent),
                    className: fileNameNoExtension,
                    rootNamespace: Path.GetFileName(rootNamespace),
                    sourceFileName: fileName);

            var generatedCode = generatorResults.GeneratedCode;
            return generatedCode;
        }
    }
}
