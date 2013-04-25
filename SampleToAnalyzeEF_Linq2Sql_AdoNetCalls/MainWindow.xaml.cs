﻿using System;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;
using System.Linq;
using System.Windows;
using Linq;


namespace SampleToAnalyzeEF_Linq2Sql_AdoNetCalls
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void RunEF4SampleButton_Click(object sender, RoutedEventArgs e){
            ResultTextBox.Text = string.Empty;
            using (var ctx = new Test_RalfEntities()){
                var result = ctx.Order.Where(n => n.Id > 1);
                foreach (var order in result){
                    var line = CreateResultTextLine(order.Id, order.OrderName, order.OrderDate, order.Total);
                    ResultTextBox.Text += line;
                }
            }
        }

        

        private void RunLinq2SqlSampleButton_Click(object sender, RoutedEventArgs e)
        {
            ResultTextBox.Text = string.Empty;
            using (var ctx = new OrderModelsLinqDataContext())
            {
                var result = ctx.LinqOrders.Where(n => n.Id > 1);
                foreach (var order in result)
                {
                    var line = CreateResultTextLine(order.Id, order.OrderName, order.OrderDate, order.Total);
                    ResultTextBox.Text += line;
                }
            }
        }

        private void RunAdoNetSampleButton_Click(object sender, RoutedEventArgs e){
            ResultTextBox.Text = string.Empty;
            var conStrName = "SampleToAnalyzeEF_Linq2Sql_AdoNetCalls.Properties.Settings.Test_RalfConnectionString";
            var connStr = ConfigurationManager.ConnectionStrings[conStrName].ConnectionString;
            var conn = new SqlConnection(connStr);
            var cmd = conn.CreateCommand();
            cmd.CommandText = "select Id, OrderName, OrderDate, Total from [dbo].[Order] where Id>@Id";
            cmd.Parameters.Add("@Id", SqlDbType.Int).Value = 1;
            conn.Open();
            var reader = cmd.ExecuteReader(CommandBehavior.CloseConnection);
            {
                while (reader.Read()){
                    var line = CreateResultTextLine(Convert.ToInt32(reader["Id"]),
                                                    (string) reader["OrderName"],
                                                    Convert.ToDateTime(reader["OrderDate"]),
                                                    Convert.ToDecimal(reader["Total"]));
                    ResultTextBox.Text += line;
                }
            }

        }

        private static string CreateResultTextLine(int id, string name, DateTime? date, decimal? total){
            return string.Format("{0} - {1} - {2} - {3}\n", id, name, 
                date.HasValue?date.Value:DateTime.MinValue, 
                total.HasValue?total.Value:total.GetValueOrDefault(0));
        }
    }
}