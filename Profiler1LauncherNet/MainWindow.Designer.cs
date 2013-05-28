namespace Profiler1LauncherNet
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SelectedProcessToProfileTextbox = new System.Windows.Forms.TextBox();
            this.SelectApplicationButton = new System.Windows.Forms.Button();
            this.StartProfilingButton = new System.Windows.Forms.Button();
            this.SelectProcessToProfileFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.MessageLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // SelectedProcessToProfileTextbox
            // 
            this.SelectedProcessToProfileTextbox.Location = new System.Drawing.Point(13, 13);
            this.SelectedProcessToProfileTextbox.Name = "SelectedProcessToProfileTextbox";
            this.SelectedProcessToProfileTextbox.Size = new System.Drawing.Size(416, 20);
            this.SelectedProcessToProfileTextbox.TabIndex = 0;
            // 
            // SelectApplicationButton
            // 
            this.SelectApplicationButton.Location = new System.Drawing.Point(435, 10);
            this.SelectApplicationButton.Name = "SelectApplicationButton";
            this.SelectApplicationButton.Size = new System.Drawing.Size(135, 23);
            this.SelectApplicationButton.TabIndex = 1;
            this.SelectApplicationButton.Text = "Select Application ";
            this.SelectApplicationButton.UseVisualStyleBackColor = true;
            this.SelectApplicationButton.Click += new System.EventHandler(this.SelectApplicationButton_Click);
            // 
            // StartProfilingButton
            // 
            this.StartProfilingButton.Location = new System.Drawing.Point(435, 39);
            this.StartProfilingButton.Name = "StartProfilingButton";
            this.StartProfilingButton.Size = new System.Drawing.Size(135, 23);
            this.StartProfilingButton.TabIndex = 2;
            this.StartProfilingButton.Text = "Start Profiling";
            this.StartProfilingButton.UseVisualStyleBackColor = true;
            this.StartProfilingButton.Click += new System.EventHandler(this.StartProfilingButton_Click);
            // 
            // SelectProcessToProfileFileDialog
            // 
            this.SelectProcessToProfileFileDialog.Filter = ".Net Executables|*.exe";
            this.SelectProcessToProfileFileDialog.Title = "Select process";
            // 
            // MessageLabel
            // 
            this.MessageLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.MessageLabel.AutoSize = true;
            this.MessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MessageLabel.ForeColor = System.Drawing.SystemColors.Highlight;
            this.MessageLabel.Location = new System.Drawing.Point(13, 109);
            this.MessageLabel.Name = "MessageLabel";
            this.MessageLabel.Size = new System.Drawing.Size(65, 17);
            this.MessageLabel.TabIndex = 3;
            this.MessageLabel.Text = "Message";
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(582, 256);
            this.Controls.Add(this.MessageLabel);
            this.Controls.Add(this.StartProfilingButton);
            this.Controls.Add(this.SelectApplicationButton);
            this.Controls.Add(this.SelectedProcessToProfileTextbox);
            this.Name = "MainWindow";
            this.Text = "MainWindow - Profiler Launcher";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox SelectedProcessToProfileTextbox;
        private System.Windows.Forms.Button SelectApplicationButton;
        private System.Windows.Forms.Button StartProfilingButton;
        private System.Windows.Forms.OpenFileDialog SelectProcessToProfileFileDialog;
        private System.Windows.Forms.Label MessageLabel;
    }
}

