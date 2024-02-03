using System;
using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

namespace Inferno
{
    [Generate]
    public class EditorProject : InfernoApp
    {
        public EditorProject() : base("editor")
        {
            SourceRootPath = Defines.Paths.INFERNO_EDITOR_SRC;
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.AddPrivateDependency<CoreLib>(target);
            conf.AddPrivateDependency<GraphicsD3D12Lib>(target);

            conf.IncludePaths.Add(Defines.Paths.INFERNO_CORE);
            conf.IncludePaths.Add(Defines.Paths.INFERNO_GRAPHICS);
        }
    }
}