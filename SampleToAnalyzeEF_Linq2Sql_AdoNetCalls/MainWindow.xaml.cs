using System;

using System.Linq;
using System.Windows;
using Linq;
using SampleToAnalyzeEF_Linq2Sql;


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
    using (var ctx = new Test_Entities()){
        var result = ctx.Order.Where(n => n.Id > 0 && n.OrderDate>new DateTime(2012,12,21) && n.OrderName!="Order2");
        foreach (var order in result){
            var line = CreateResultTextLine(order.Id, order.OrderName, order.OrderDate, order.Total);
            ResultTextBox.Text += line;
        }
    }
}

private void RunLinq2SqlSampleButton_Click(object sender, RoutedEventArgs e){
    ResultTextBox.Text = string.Empty;
    using (var ctx = new OrderModelsLinqDataContext()){
        var result = ctx.LinqOrders.Where(n => n.Id > 0 && n.OrderDate > new DateTime(2012, 12, 21) && n.OrderName != "Order2");
        foreach (var order in result){
            var line = CreateResultTextLine(order.Id, order.OrderName, order.OrderDate, order.Total);
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
