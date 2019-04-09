namespace Analysis
{
    partial class analysis
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.load2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unload2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.refToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadRef1 = new System.Windows.Forms.ToolStripMenuItem();
            this.loadRef2 = new System.Windows.Forms.ToolStripMenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.HighLowOpen_EnterPert = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.HighLowOpen_StopPert = new System.Windows.Forms.TextBox();
            this.HighLowOpenStart = new System.Windows.Forms.Button();
            this.analysisResult = new System.Windows.Forms.RichTextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.HighLowOpen_lastDays = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.ld_stopPert = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.ld_changePert = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.ld_lastDays = new System.Windows.Forms.TextBox();
            this.LastDayChangeStart = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.buyStop_enterPert = new System.Windows.Forms.TextBox();
            this.sellHighStar = new System.Windows.Forms.Button();
            this.hiLowAnalysis = new System.Windows.Forms.Button();
            this.HighLowOpen_EnterPert2 = new System.Windows.Forms.TextBox();
            this.HighLowOpen_StopPert2 = new System.Windows.Forms.TextBox();
            this.fileName1 = new System.Windows.Forms.Label();
            this.fileName2 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.LastDayLongShort = new System.Windows.Forms.TextBox();
            this.lastDayAnalysis = new System.Windows.Forms.Button();
            this.label12 = new System.Windows.Forms.Label();
            this.buyStopLastDays = new System.Windows.Forms.TextBox();
            this.pertCrossOverAnalysis = new System.Windows.Forms.Button();
            this.OpenHighSellLimitAnalysis = new System.Windows.Forms.Button();
            this.label15 = new System.Windows.Forms.Label();
            this.buyHigh_LastDays = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.buyHigh_EnterPert = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.buyHigh_StopPert = new System.Windows.Forms.TextBox();
            this.OpenHighSellLimit_Start = new System.Windows.Forms.Button();
            this.buyStop_stopPert = new System.Windows.Forms.TextBox();
            this.highLow_Display = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.buyHigh_OpenPert = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.longInvest_days = new System.Windows.Forms.TextBox();
            this.label21 = new System.Windows.Forms.Label();
            this.longInvest_buyPert = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.longInvest_stopPert = new System.Windows.Forms.TextBox();
            this.longInvest_Start = new System.Windows.Forms.Button();
            this.longInvest_Analysis = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.refToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1083, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadToolStripMenuItem,
            this.load2ToolStripMenuItem,
            this.unload2ToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // loadToolStripMenuItem
            // 
            this.loadToolStripMenuItem.Name = "loadToolStripMenuItem";
            this.loadToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.loadToolStripMenuItem.Text = "Load";
            this.loadToolStripMenuItem.Click += new System.EventHandler(this.clickLoad);
            // 
            // load2ToolStripMenuItem
            // 
            this.load2ToolStripMenuItem.Name = "load2ToolStripMenuItem";
            this.load2ToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.load2ToolStripMenuItem.Text = "load2";
            this.load2ToolStripMenuItem.Click += new System.EventHandler(this.load2ToolStripMenuItem_Click);
            // 
            // unload2ToolStripMenuItem
            // 
            this.unload2ToolStripMenuItem.Name = "unload2ToolStripMenuItem";
            this.unload2ToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.unload2ToolStripMenuItem.Text = "unload2";
            this.unload2ToolStripMenuItem.Click += new System.EventHandler(this.unload2ToolStripMenuItem_Click);
            // 
            // refToolStripMenuItem
            // 
            this.refToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadRef1,
            this.loadRef2});
            this.refToolStripMenuItem.Name = "refToolStripMenuItem";
            this.refToolStripMenuItem.Size = new System.Drawing.Size(36, 20);
            this.refToolStripMenuItem.Text = "Ref";
            // 
            // loadRef1
            // 
            this.loadRef1.Name = "loadRef1";
            this.loadRef1.Size = new System.Drawing.Size(126, 22);
            this.loadRef1.Text = "Load Ref1";
            this.loadRef1.Click += new System.EventHandler(this.loadRef1_Click);
            // 
            // loadRef2
            // 
            this.loadRef2.Name = "loadRef2";
            this.loadRef2.Size = new System.Drawing.Size(126, 22);
            this.loadRef2.Text = "Load Ref2";
            this.loadRef2.Click += new System.EventHandler(this.loadRef2_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(25, 95);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(106, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "High/Low open:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label2.Location = new System.Drawing.Point(284, 95);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(67, 17);
            this.label2.TabIndex = 2;
            this.label2.Text = "enterPert";
            // 
            // HighLowOpen_EnterPert
            // 
            this.HighLowOpen_EnterPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.HighLowOpen_EnterPert.Location = new System.Drawing.Point(371, 92);
            this.HighLowOpen_EnterPert.Name = "HighLowOpen_EnterPert";
            this.HighLowOpen_EnterPert.Size = new System.Drawing.Size(55, 23);
            this.HighLowOpen_EnterPert.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label3.Location = new System.Drawing.Point(445, 95);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(61, 17);
            this.label3.TabIndex = 4;
            this.label3.Text = "stopPert";
            // 
            // HighLowOpen_StopPert
            // 
            this.HighLowOpen_StopPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.HighLowOpen_StopPert.Location = new System.Drawing.Point(515, 92);
            this.HighLowOpen_StopPert.Name = "HighLowOpen_StopPert";
            this.HighLowOpen_StopPert.Size = new System.Drawing.Size(69, 23);
            this.HighLowOpen_StopPert.TabIndex = 3;
            // 
            // HighLowOpenStart
            // 
            this.HighLowOpenStart.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.HighLowOpenStart.Location = new System.Drawing.Point(744, 95);
            this.HighLowOpenStart.Name = "HighLowOpenStart";
            this.HighLowOpenStart.Size = new System.Drawing.Size(75, 23);
            this.HighLowOpenStart.TabIndex = 5;
            this.HighLowOpenStart.Text = "start";
            this.HighLowOpenStart.UseVisualStyleBackColor = true;
            this.HighLowOpenStart.Click += new System.EventHandler(this.HighLowOpenStart_Click);
            // 
            // analysisResult
            // 
            this.analysisResult.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.analysisResult.Location = new System.Drawing.Point(12, 340);
            this.analysisResult.Name = "analysisResult";
            this.analysisResult.Size = new System.Drawing.Size(1059, 598);
            this.analysisResult.TabIndex = 7;
            this.analysisResult.Text = "";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label4.Location = new System.Drawing.Point(131, 95);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(62, 17);
            this.label4.TabIndex = 8;
            this.label4.Text = "lastDays";
            // 
            // HighLowOpen_lastDays
            // 
            this.HighLowOpen_lastDays.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.HighLowOpen_lastDays.Location = new System.Drawing.Point(195, 92);
            this.HighLowOpen_lastDays.Name = "HighLowOpen_lastDays";
            this.HighLowOpen_lastDays.Size = new System.Drawing.Size(75, 23);
            this.HighLowOpen_lastDays.TabIndex = 1;
            this.HighLowOpen_lastDays.Text = "0";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label5.Location = new System.Drawing.Point(445, 159);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 17);
            this.label5.TabIndex = 10;
            this.label5.Text = "stopPert";
            // 
            // ld_stopPert
            // 
            this.ld_stopPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ld_stopPert.Location = new System.Drawing.Point(515, 156);
            this.ld_stopPert.Name = "ld_stopPert";
            this.ld_stopPert.Size = new System.Drawing.Size(69, 23);
            this.ld_stopPert.TabIndex = 8;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(284, 162);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(81, 17);
            this.label6.TabIndex = 11;
            this.label6.Text = "changePert";
            // 
            // ld_changePert
            // 
            this.ld_changePert.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ld_changePert.Location = new System.Drawing.Point(371, 159);
            this.ld_changePert.Name = "ld_changePert";
            this.ld_changePert.Size = new System.Drawing.Size(55, 23);
            this.ld_changePert.TabIndex = 7;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(23, 162);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(113, 17);
            this.label7.TabIndex = 12;
            this.label7.Text = "LastDayChange:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(131, 162);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(62, 17);
            this.label8.TabIndex = 13;
            this.label8.Text = "lastDays";
            // 
            // ld_lastDays
            // 
            this.ld_lastDays.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ld_lastDays.Location = new System.Drawing.Point(195, 159);
            this.ld_lastDays.Name = "ld_lastDays";
            this.ld_lastDays.Size = new System.Drawing.Size(75, 23);
            this.ld_lastDays.TabIndex = 6;
            this.ld_lastDays.Text = "0";
            // 
            // LastDayChangeStart
            // 
            this.LastDayChangeStart.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LastDayChangeStart.Location = new System.Drawing.Point(744, 158);
            this.LastDayChangeStart.Name = "LastDayChangeStart";
            this.LastDayChangeStart.Size = new System.Drawing.Size(75, 23);
            this.LastDayChangeStart.TabIndex = 9;
            this.LastDayChangeStart.Text = "start";
            this.LastDayChangeStart.UseVisualStyleBackColor = true;
            this.LastDayChangeStart.Click += new System.EventHandler(this.lastDayChangeStart_Click);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(284, 199);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(68, 17);
            this.label11.TabIndex = 16;
            this.label11.Text = "EnterPert";
            // 
            // buyStop_enterPert
            // 
            this.buyStop_enterPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buyStop_enterPert.Location = new System.Drawing.Point(371, 197);
            this.buyStop_enterPert.Name = "buyStop_enterPert";
            this.buyStop_enterPert.Size = new System.Drawing.Size(49, 23);
            this.buyStop_enterPert.TabIndex = 19;
            this.buyStop_enterPert.Text = "0.01";
            // 
            // sellHighStar
            // 
            this.sellHighStar.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.sellHighStar.Location = new System.Drawing.Point(745, 197);
            this.sellHighStar.Name = "sellHighStar";
            this.sellHighStar.Size = new System.Drawing.Size(75, 23);
            this.sellHighStar.TabIndex = 22;
            this.sellHighStar.Text = "start";
            this.sellHighStar.UseVisualStyleBackColor = true;
            this.sellHighStar.Click += new System.EventHandler(this.SellHighStart_Click);
            // 
            // hiLowAnalysis
            // 
            this.hiLowAnalysis.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.hiLowAnalysis.Location = new System.Drawing.Point(847, 95);
            this.hiLowAnalysis.Name = "hiLowAnalysis";
            this.hiLowAnalysis.Size = new System.Drawing.Size(75, 23);
            this.hiLowAnalysis.TabIndex = 23;
            this.hiLowAnalysis.Text = "Analysis";
            this.hiLowAnalysis.UseVisualStyleBackColor = true;
            this.hiLowAnalysis.Click += new System.EventHandler(this.hiLowAnalysis_Click);
            // 
            // HighLowOpen_EnterPert2
            // 
            this.HighLowOpen_EnterPert2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.HighLowOpen_EnterPert2.Location = new System.Drawing.Point(371, 124);
            this.HighLowOpen_EnterPert2.Name = "HighLowOpen_EnterPert2";
            this.HighLowOpen_EnterPert2.Size = new System.Drawing.Size(55, 23);
            this.HighLowOpen_EnterPert2.TabIndex = 24;
            this.HighLowOpen_EnterPert2.Text = "0";
            // 
            // HighLowOpen_StopPert2
            // 
            this.HighLowOpen_StopPert2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.HighLowOpen_StopPert2.Location = new System.Drawing.Point(515, 124);
            this.HighLowOpen_StopPert2.Name = "HighLowOpen_StopPert2";
            this.HighLowOpen_StopPert2.Size = new System.Drawing.Size(69, 23);
            this.HighLowOpen_StopPert2.TabIndex = 25;
            this.HighLowOpen_StopPert2.Text = "0";
            // 
            // fileName1
            // 
            this.fileName1.AutoSize = true;
            this.fileName1.Location = new System.Drawing.Point(38, 28);
            this.fileName1.Name = "fileName1";
            this.fileName1.Size = new System.Drawing.Size(0, 13);
            this.fileName1.TabIndex = 26;
            // 
            // fileName2
            // 
            this.fileName2.AutoSize = true;
            this.fileName2.Location = new System.Drawing.Point(38, 63);
            this.fileName2.Name = "fileName2";
            this.fileName2.Size = new System.Drawing.Size(0, 13);
            this.fileName2.TabIndex = 27;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label14.Location = new System.Drawing.Point(598, 156);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(74, 17);
            this.label14.TabIndex = 28;
            this.label14.Text = "LongShort";
            // 
            // LastDayLongShort
            // 
            this.LastDayLongShort.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LastDayLongShort.Location = new System.Drawing.Point(677, 156);
            this.LastDayLongShort.Name = "LastDayLongShort";
            this.LastDayLongShort.Size = new System.Drawing.Size(51, 23);
            this.LastDayLongShort.TabIndex = 29;
            this.LastDayLongShort.Text = "1";
            // 
            // lastDayAnalysis
            // 
            this.lastDayAnalysis.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lastDayAnalysis.Location = new System.Drawing.Point(847, 158);
            this.lastDayAnalysis.Name = "lastDayAnalysis";
            this.lastDayAnalysis.Size = new System.Drawing.Size(75, 23);
            this.lastDayAnalysis.TabIndex = 30;
            this.lastDayAnalysis.Text = "Analysis";
            this.lastDayAnalysis.UseVisualStyleBackColor = true;
            this.lastDayAnalysis.Click += new System.EventHandler(this.lastDayAnalysis_Click);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(127, 200);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(62, 17);
            this.label12.TabIndex = 31;
            this.label12.Text = "lastDays";
            // 
            // buyStopLastDays
            // 
            this.buyStopLastDays.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.buyStopLastDays.Location = new System.Drawing.Point(195, 197);
            this.buyStopLastDays.Name = "buyStopLastDays";
            this.buyStopLastDays.Size = new System.Drawing.Size(78, 23);
            this.buyStopLastDays.TabIndex = 32;
            this.buyStopLastDays.Text = "0";
            // 
            // pertCrossOverAnalysis
            // 
            this.pertCrossOverAnalysis.Location = new System.Drawing.Point(847, 196);
            this.pertCrossOverAnalysis.Name = "pertCrossOverAnalysis";
            this.pertCrossOverAnalysis.Size = new System.Drawing.Size(75, 23);
            this.pertCrossOverAnalysis.TabIndex = 33;
            this.pertCrossOverAnalysis.Text = "Analysis";
            this.pertCrossOverAnalysis.UseVisualStyleBackColor = true;
            this.pertCrossOverAnalysis.Click += new System.EventHandler(this.pertCrossOverAnalysis_Click);
            // 
            // OpenHighSellLimitAnalysis
            // 
            this.OpenHighSellLimitAnalysis.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OpenHighSellLimitAnalysis.Location = new System.Drawing.Point(847, 238);
            this.OpenHighSellLimitAnalysis.Name = "OpenHighSellLimitAnalysis";
            this.OpenHighSellLimitAnalysis.Size = new System.Drawing.Size(75, 23);
            this.OpenHighSellLimitAnalysis.TabIndex = 34;
            this.OpenHighSellLimitAnalysis.Text = "Analysis";
            this.OpenHighSellLimitAnalysis.UseVisualStyleBackColor = true;
            this.OpenHighSellLimitAnalysis.Click += new System.EventHandler(this.OpenHighAnalysis_Click);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label15.Location = new System.Drawing.Point(131, 238);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(61, 16);
            this.label15.TabIndex = 35;
            this.label15.Text = "lastDays";
            // 
            // buyHigh_LastDays
            // 
            this.buyHigh_LastDays.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buyHigh_LastDays.Location = new System.Drawing.Point(195, 235);
            this.buyHigh_LastDays.Name = "buyHigh_LastDays";
            this.buyHigh_LastDays.Size = new System.Drawing.Size(75, 22);
            this.buyHigh_LastDays.TabIndex = 36;
            this.buyHigh_LastDays.Text = "0";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label16.Location = new System.Drawing.Point(428, 238);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(62, 16);
            this.label16.TabIndex = 37;
            this.label16.Text = "enterPert";
            // 
            // buyHigh_EnterPert
            // 
            this.buyHigh_EnterPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buyHigh_EnterPert.Location = new System.Drawing.Point(501, 238);
            this.buyHigh_EnterPert.Name = "buyHigh_EnterPert";
            this.buyHigh_EnterPert.Size = new System.Drawing.Size(67, 22);
            this.buyHigh_EnterPert.TabIndex = 38;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(582, 240);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(58, 16);
            this.label17.TabIndex = 39;
            this.label17.Text = "stopPert";
            // 
            // buyHigh_StopPert
            // 
            this.buyHigh_StopPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buyHigh_StopPert.Location = new System.Drawing.Point(648, 238);
            this.buyHigh_StopPert.Name = "buyHigh_StopPert";
            this.buyHigh_StopPert.Size = new System.Drawing.Size(69, 22);
            this.buyHigh_StopPert.TabIndex = 40;
            // 
            // OpenHighSellLimit_Start
            // 
            this.OpenHighSellLimit_Start.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OpenHighSellLimit_Start.Location = new System.Drawing.Point(744, 238);
            this.OpenHighSellLimit_Start.Name = "OpenHighSellLimit_Start";
            this.OpenHighSellLimit_Start.Size = new System.Drawing.Size(75, 23);
            this.OpenHighSellLimit_Start.TabIndex = 41;
            this.OpenHighSellLimit_Start.Text = "Start";
            this.OpenHighSellLimit_Start.UseVisualStyleBackColor = true;
            this.OpenHighSellLimit_Start.Click += new System.EventHandler(this.OpenHighSellLimit_Start_Click);
            // 
            // buyStop_stopPert
            // 
            this.buyStop_stopPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.buyStop_stopPert.Location = new System.Drawing.Point(510, 197);
            this.buyStop_stopPert.Name = "buyStop_stopPert";
            this.buyStop_stopPert.Size = new System.Drawing.Size(58, 23);
            this.buyStop_stopPert.TabIndex = 20;
            this.buyStop_stopPert.Text = "0.01";
            // 
            // highLow_Display
            // 
            this.highLow_Display.Location = new System.Drawing.Point(928, 97);
            this.highLow_Display.Name = "highLow_Display";
            this.highLow_Display.Size = new System.Drawing.Size(75, 23);
            this.highLow_Display.TabIndex = 42;
            this.highLow_Display.Text = "Display";
            this.highLow_Display.UseVisualStyleBackColor = true;
            this.highLow_Display.Click += new System.EventHandler(this.highLow_Display_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(275, 238);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(66, 16);
            this.label10.TabIndex = 43;
            this.label10.Text = "openPert:";
            // 
            // buyHigh_OpenPert
            // 
            this.buyHigh_OpenPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buyHigh_OpenPert.Location = new System.Drawing.Point(344, 237);
            this.buyHigh_OpenPert.Name = "buyHigh_OpenPert";
            this.buyHigh_OpenPert.Size = new System.Drawing.Size(78, 22);
            this.buyHigh_OpenPert.TabIndex = 38;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(59, 240);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(50, 13);
            this.label18.TabIndex = 44;
            this.label18.Text = "buy high:";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(59, 285);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(62, 13);
            this.label19.TabIndex = 45;
            this.label19.Text = "Long invest";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label20.Location = new System.Drawing.Point(133, 285);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(61, 16);
            this.label20.TabIndex = 46;
            this.label20.Text = "lastDays";
            // 
            // longInvest_days
            // 
            this.longInvest_days.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.longInvest_days.Location = new System.Drawing.Point(195, 282);
            this.longInvest_days.Name = "longInvest_days";
            this.longInvest_days.Size = new System.Drawing.Size(75, 22);
            this.longInvest_days.TabIndex = 47;
            this.longInvest_days.Text = "0";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label21.Location = new System.Drawing.Point(279, 285);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(54, 16);
            this.label21.TabIndex = 48;
            this.label21.Text = "buyPert";
            // 
            // longInvest_buyPert
            // 
            this.longInvest_buyPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.longInvest_buyPert.Location = new System.Drawing.Point(344, 282);
            this.longInvest_buyPert.Name = "longInvest_buyPert";
            this.longInvest_buyPert.Size = new System.Drawing.Size(67, 22);
            this.longInvest_buyPert.TabIndex = 49;
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label22.Location = new System.Drawing.Point(428, 283);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(58, 16);
            this.label22.TabIndex = 50;
            this.label22.Text = "stopPert";
            // 
            // longInvest_stopPert
            // 
            this.longInvest_stopPert.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.longInvest_stopPert.Location = new System.Drawing.Point(499, 280);
            this.longInvest_stopPert.Name = "longInvest_stopPert";
            this.longInvest_stopPert.Size = new System.Drawing.Size(69, 22);
            this.longInvest_stopPert.TabIndex = 51;
            // 
            // longInvest_Start
            // 
            this.longInvest_Start.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.longInvest_Start.Location = new System.Drawing.Point(745, 282);
            this.longInvest_Start.Name = "longInvest_Start";
            this.longInvest_Start.Size = new System.Drawing.Size(75, 23);
            this.longInvest_Start.TabIndex = 52;
            this.longInvest_Start.Text = "Start";
            this.longInvest_Start.UseVisualStyleBackColor = true;
            this.longInvest_Start.Click += new System.EventHandler(this.longInvest_Start_Click);
            // 
            // longInvest_Analysis
            // 
            this.longInvest_Analysis.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.longInvest_Analysis.Location = new System.Drawing.Point(847, 282);
            this.longInvest_Analysis.Name = "longInvest_Analysis";
            this.longInvest_Analysis.Size = new System.Drawing.Size(75, 23);
            this.longInvest_Analysis.TabIndex = 53;
            this.longInvest_Analysis.Text = "Analysis";
            this.longInvest_Analysis.UseVisualStyleBackColor = true;
            this.longInvest_Analysis.Click += new System.EventHandler(this.longInvest_Analysis_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(54, 200);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(67, 17);
            this.label9.TabIndex = 54;
            this.label9.Text = "Buy stop:";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label23.Location = new System.Drawing.Point(436, 199);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(63, 17);
            this.label23.TabIndex = 55;
            this.label23.Text = "StopPert";
            // 
            // analysis
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1083, 965);
            this.Controls.Add(this.label23);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.longInvest_Analysis);
            this.Controls.Add(this.longInvest_Start);
            this.Controls.Add(this.longInvest_stopPert);
            this.Controls.Add(this.label22);
            this.Controls.Add(this.longInvest_buyPert);
            this.Controls.Add(this.label21);
            this.Controls.Add(this.longInvest_days);
            this.Controls.Add(this.label20);
            this.Controls.Add(this.label19);
            this.Controls.Add(this.label18);
            this.Controls.Add(this.buyHigh_OpenPert);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.highLow_Display);
            this.Controls.Add(this.buyStop_stopPert);
            this.Controls.Add(this.OpenHighSellLimit_Start);
            this.Controls.Add(this.buyHigh_StopPert);
            this.Controls.Add(this.label17);
            this.Controls.Add(this.buyHigh_EnterPert);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.buyHigh_LastDays);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.OpenHighSellLimitAnalysis);
            this.Controls.Add(this.pertCrossOverAnalysis);
            this.Controls.Add(this.buyStopLastDays);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.lastDayAnalysis);
            this.Controls.Add(this.LastDayLongShort);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.fileName2);
            this.Controls.Add(this.fileName1);
            this.Controls.Add(this.HighLowOpen_StopPert2);
            this.Controls.Add(this.HighLowOpen_EnterPert2);
            this.Controls.Add(this.hiLowAnalysis);
            this.Controls.Add(this.sellHighStar);
            this.Controls.Add(this.buyStop_enterPert);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.LastDayChangeStart);
            this.Controls.Add(this.ld_lastDays);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.ld_changePert);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.ld_stopPert);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.HighLowOpen_lastDays);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.analysisResult);
            this.Controls.Add(this.HighLowOpenStart);
            this.Controls.Add(this.HighLowOpen_StopPert);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.HighLowOpen_EnterPert);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "analysis";
            this.Text = "Analysis";
            this.Load += new System.EventHandler(this.analysis_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadToolStripMenuItem;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox HighLowOpen_EnterPert;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox HighLowOpen_StopPert;
        private System.Windows.Forms.Button HighLowOpenStart;
        private System.Windows.Forms.RichTextBox analysisResult;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox HighLowOpen_lastDays;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox ld_stopPert;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox ld_changePert;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox ld_lastDays;
        private System.Windows.Forms.Button LastDayChangeStart;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox buyStop_enterPert;
        private System.Windows.Forms.Button sellHighStar;
        private System.Windows.Forms.Button hiLowAnalysis;
        private System.Windows.Forms.ToolStripMenuItem load2ToolStripMenuItem;
        private System.Windows.Forms.TextBox HighLowOpen_EnterPert2;
        private System.Windows.Forms.TextBox HighLowOpen_StopPert2;
        private System.Windows.Forms.ToolStripMenuItem unload2ToolStripMenuItem;
        private System.Windows.Forms.Label fileName1;
        private System.Windows.Forms.Label fileName2;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox LastDayLongShort;
        private System.Windows.Forms.Button lastDayAnalysis;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox buyStopLastDays;
        private System.Windows.Forms.Button pertCrossOverAnalysis;
        private System.Windows.Forms.Button OpenHighSellLimitAnalysis;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox buyHigh_LastDays;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox buyHigh_EnterPert;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox buyHigh_StopPert;
        private System.Windows.Forms.Button OpenHighSellLimit_Start;
        private System.Windows.Forms.TextBox buyStop_stopPert;
        private System.Windows.Forms.ToolStripMenuItem refToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadRef1;
        private System.Windows.Forms.ToolStripMenuItem loadRef2;
        private System.Windows.Forms.Button highLow_Display;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox buyHigh_OpenPert;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.TextBox longInvest_days;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.TextBox longInvest_buyPert;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.TextBox longInvest_stopPert;
        private System.Windows.Forms.Button longInvest_Start;
        private System.Windows.Forms.Button longInvest_Analysis;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label23;
    }
}

