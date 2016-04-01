﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using IniParser;
using IniParser.Model;
using Microsoft.WindowsAPICodePack.Shell;

namespace TemplePlusConfig
{
    /// <summary>
    ///     Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly string _iniPath;

        private readonly IniViewModel _iniViewModel = new IniViewModel();

        public MainWindow()
        {
            InitializeComponent();

            DataContext = _iniViewModel;

            var saveGameFolder = KnownFolders.SavedGames.Path;
            _iniPath = Path.Combine(saveGameFolder, "TemplePlus", "TemplePlus.ini");

            if (File.Exists(_iniPath))
            {
                var iniParser = new FileIniDataParser();
                var iniData = iniParser.ReadFile(_iniPath, Encoding.UTF8);
                _iniViewModel.LoadFromIni(iniData);
            }

            if (string.IsNullOrEmpty(InstallationDir.InstallationPath))
            {
                InstallationDir.AutoDetectInstallation();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            IniData iniData;
            var iniParser = new FileIniDataParser();
            iniParser.Parser.Configuration.AssigmentSpacer = "";

            if (File.Exists(_iniPath))
            {
                iniData = iniParser.ReadFile(_iniPath, Encoding.UTF8);
            }
            else
            {
                iniData = new IniData();
            }

            var iniDir = Path.GetDirectoryName(_iniPath);
            if (iniDir != null)
            {
                Directory.CreateDirectory(iniDir);
            }

            // TODO: Check why two-way binding in this particular case is not working
            _iniViewModel.InstallationPath = InstallationDir.InstallationPath;
            _iniViewModel.SaveToIni(iniData);

            // Disable BOM otherwise TP barfs when loading the ini
            iniParser.WriteFile(_iniPath, iniData, new UTF8Encoding(false));

            Close();
        }
    }
}