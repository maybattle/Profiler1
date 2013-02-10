using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FirstProfilerNet
{
    public partial class MainWindow : Form
    {
        
        public MainWindow()
        {
            InitializeComponent();
            SelectedProcessToProfileTextbox.Text = @"C:\TestProjects\Profiler1\bin\Debug\SampleToProfile.exe";
        }

        private void SelectApplicationButton_Click(object sender, EventArgs e)
        {
            if (SelectProcessToProfileFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                SelectedProcessToProfileTextbox.Text = SelectProcessToProfileFileDialog.FileName;
            }
        }

        private void StartProfilingButton_Click(object sender, EventArgs e)
        {
            var startInfo = new ProcessStartInfo(SelectedProcessToProfileTextbox.Text);
            //Profiling fuer den Prozess erlauben
            startInfo.EnvironmentVariables.Add("COR_ENABLE_PROFILING","1");
            //Bis CLR 4
            startInfo.EnvironmentVariables.Add("COR_PROFILER", "{DFE1C658-1DB9-49A4-9BE4-D08E9A65653D}");
            //Ab CLR 4
            //Pfad zur Profiler DLL, die von der CLR aktiviert werden soll.
            startInfo.EnvironmentVariables.Add("COR_PROFILER_PATH", @"C:\TestProjects\Profiler1\bin\Debug\Profiler1.dll");
            startInfo.UseShellExecute = false;
            var process = Process.Start(startInfo);
        }
    }
}
