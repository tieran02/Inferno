using System.IO; // For Path.Combine
using System;
using Sharpmake; // Contains the entire Sharpmake object library.
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Linq;

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
                string filename = Path.GetFileName(vertexEntry);
                filename = Path.ChangeExtension(filename, ".dxil");
                string dest = Path.Combine(outputDir, filename);
                cmd += $"{DXC} /Zi -T vs_6_3 -Fo {dest} {vertexEntry}\n";
            }

            //Pixel shaders
            string [] pixelEntries = Directory.GetFiles(inputDir, "*.pixel.hlsl", SearchOption.AllDirectories);
            foreach(string pixelEntry in pixelEntries)
            {
                string filename = Path.GetFileName(pixelEntry);
                filename = Path.ChangeExtension(filename, ".dxil");
                string dest = Path.Combine(outputDir, filename);
                cmd += $"{DXC} /Zi -T ps_6_3 -Fo {dest} {pixelEntry}\n";
            }

            //combinedShaders shaders
            Regex reg = new Regex(@"^(?!.*\.(pixel|vert)\.hlsl$).*\.hlsl$");
            string [] combinedEntries = Directory.GetFiles(inputDir, "*.hlsl", SearchOption.AllDirectories).Where(path => reg.IsMatch(path)).ToArray ();
            foreach(string entry in combinedEntries)
            {
                Console.WriteLine(entry);
                string filename = Path.GetFileName(entry);

                //Vertex shader
                string vertexFilename = Path.ChangeExtension(filename, "vert.dxil");
                string dest = Path.Combine(outputDir, vertexFilename);
                cmd += $"{DXC} /Zi -E VSmain -T vs_6_3 -Fo {dest} {entry}\n";

                //Pixel shader
                string pixelFilename = Path.ChangeExtension(filename, "pixel.dxil");
                dest = Path.Combine(outputDir, pixelFilename);
                cmd += $"{DXC} /Zi -E PSmain -T ps_6_3 -Fo {dest} {entry}\n";
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