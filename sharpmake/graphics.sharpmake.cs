using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

namespace Inferno
{
    // Represents the project that will be generated by Sharpmake and that contains
    // the sample C++ code.
    [Generate]
    public class GraphicsLib : InfernoLib
    {
        public GraphicsLib() : base("graphics")
        {
            SourceRootPath = Defines.Paths.INFERNO_GRAPHICS_SRC;
            AdditionalSourceRootPaths.Add(Defines.Paths.INFERNO_GRAPHICS_INC);
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(Defines.Paths.INFERNO_GRAPHICS_INC);

            conf.AddPublicDependency<CoreLib>(target);
            conf.IncludePaths.Add(Defines.Paths.INFERNO_CORE_INC);

            conf.AddPublicDependency<IMGUILib>(target);
            conf.AddPublicDependency<GraphicsD3D12Lib>(target);

            conf.IncludePaths.Add(GraphicsD3D12Lib.INFERNO_GRAPHICS_D3D12_INC);
            conf.IncludePaths.Add(Path.Combine(Defines.Paths.INFERNO_THIRD_PARTY, @"directxheaders/include"));
            conf.IncludePaths.Add(Path.Combine(Defines.Paths.INFERNO_THIRD_PARTY, @"stb"));
            conf.IncludePaths.Add(Path.Combine(Defines.Paths.INFERNO_THIRD_PARTY, @"tinygltf"));
        }
    }

    [Generate]
    public class GraphicsD3D12Lib : InfernoLib
    {
        static public readonly string INFERNO_GRAPHICS_D3D12_SRC = Path.Combine(Defines.Paths.INFERNO_ENGINE, @"graphicsD3D12\src");
        static public readonly string INFERNO_GRAPHICS_D3D12_INC = Path.Combine(Defines.Paths.INFERNO_ENGINE, @"graphicsD3D12\include\");
        public GraphicsD3D12Lib() : base("graphics-D3D12")
        {
            SourceRootPath = INFERNO_GRAPHICS_D3D12_SRC;
            AdditionalSourceRootPaths.Add(INFERNO_GRAPHICS_D3D12_INC);
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(Defines.Paths.INFERNO_GRAPHICS_INC);
            conf.IncludePaths.Add(INFERNO_GRAPHICS_D3D12_INC);

            conf.AddPublicDependency<CoreLib>(target);
            conf.AddPublicDependency<IMGUILib>(target);

            //d3d12 headers and linking
            conf.IncludePaths.Add(Path.Combine(Defines.Paths.INFERNO_THIRD_PARTY, @"directxheaders/include"));
            conf.LibraryFiles.Add("dxcompiler");
            conf.LibraryFiles.Add("dxguid");
            conf.LibraryFiles.Add("d3d12");
            conf.LibraryFiles.Add("dxgi");
        }
    }
}