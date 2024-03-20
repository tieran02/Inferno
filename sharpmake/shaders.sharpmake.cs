using System.IO; // For Path.Combine
using System;
using Sharpmake; // Contains the entire Sharpmake object library.
using System.Collections.Generic;

namespace Inferno
{
    public class ShaderUtil
    {
        static string DXC = Path.Combine(Defines.Paths.INFERNO_THIRD_PARTY_BIN, @"dxc/dxc.exe");

        
        public static string CompileShadersComamand(string inputDir, string outputDir)
        {
            string cmd = "";
            //Vertex shaders
            string [] vertexEntries = Directory.GetFiles(inputDir, "*.vert.hlsl", SearchOption.AllDirectories);
            foreach(string vertexEntry in vertexEntries)
            {
                Console.WriteLine(vertexEntry);
                string filename = Path.GetFileName(vertexEntry);
                filename = Path.ChangeExtension(filename, ".dxil");
                string dest = Path.Combine(outputDir, filename);
                cmd += $"{DXC} /Zi -T vs_6_3 -Fo {dest} {vertexEntry}\n";
            }

            //Pixel shaders
            string [] pixelEntries = Directory.GetFiles(inputDir, "*.pixel.hlsl", SearchOption.AllDirectories);
            foreach(string pixelEntry in pixelEntries)
            {
                Console.WriteLine(pixelEntry);
                string filename = Path.GetFileName(pixelEntry);
                filename = Path.ChangeExtension(filename, ".dxil");
                string dest = Path.Combine(outputDir, filename);
                cmd += $"{DXC} /Zi -T ps_6_3 -Fo {dest} {pixelEntry}\n";
            }
            return cmd;
        }

        public static void CompileShadersPreBuild(Project.Configuration conf, string inputDir, string outputDir)
        {
            // Get information about the source directory
            var dir = new DirectoryInfo(outputDir);

            // Check if the source directory exists
            if (!dir.Exists)
                dir = Directory.CreateDirectory(outputDir);

            conf.EventPreBuild.Add(CompileShadersComamand(inputDir,outputDir));
        }
    }
}