using System;
using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.
using Inferno;

namespace Inferno.Samples
{    
    public class SampleProject : InfernoApp
    {
        static public readonly string INFERNO_SAMPLES = Path.Combine(Defines.Paths.INFERNO_ROOT, @"samples\");
        static public void AddSampleProjects(Solution.Configuration conf, Target target)
        {
            conf.AddProject<SampleHelloTriangle>(target);
            conf.AddProject<SampleConstantBuffer>(target);
            conf.AddProject<SampleRenderPass>(target);
        }


        protected readonly string SampleRootDir;
        public SampleProject(string projectName) : base(projectName)
        {
            SampleRootDir = $"{INFERNO_SAMPLES}/{projectName}";
            SourceRootPath = $"{SampleRootDir}/src";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "Samples";

            conf.AddPrivateDependency<CoreLib>(target);
            conf.AddPrivateDependency<GraphicsLib>(target);

            conf.IncludePaths.Add(Defines.Paths.INFERNO_CORE);
            conf.IncludePaths.Add(Defines.Paths.INFERNO_GRAPHICS);

            conf.PrecompHeader = "";
            conf.PrecompSource = "";

            conf.ProjectPath = Path.Combine($"{Defines.Paths.INFERNO_PROJECTS}/samples", Name);

            conf.VcxprojUserFile = new();
            conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = SampleRootDir;
        }
    }

    [Generate]
    public class SampleHelloTriangle : SampleProject
    {
        public SampleHelloTriangle() : base("hellotriangle")
        {
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            string inDataFolder = $"{SampleRootDir}/src/shaders";
            string outDataFolder = $"{SampleRootDir}/data/shaders";
            ShaderUtil.CompileShadersPreBuild(conf,inDataFolder,outDataFolder);
        }
    }

    [Generate]
    public class SampleConstantBuffer : SampleProject
    {
        public SampleConstantBuffer() : base("constantbuffer")
        {
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            string inDataFolder = $"{SampleRootDir}/src/shaders";
            string outDataFolder = $"{SampleRootDir}/data/shaders";
            ShaderUtil.CompileShadersPreBuild(conf,inDataFolder,outDataFolder);
        }
    }

    [Generate]
    public class SampleRenderPass : SampleProject
    {
        public SampleRenderPass() : base("renderpass")
        {
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            string inDataFolder = $"{SampleRootDir}/src/shaders";
            string outDataFolder = $"{SampleRootDir}/data/shaders";
            ShaderUtil.CompileShadersPreBuild(conf,inDataFolder,outDataFolder);
        }
    }

}