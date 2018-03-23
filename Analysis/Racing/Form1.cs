using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Racing
{
    
    public partial class Form1 : Form
    {
        static double TOTAL_POINT = 100;

        private List<ResultData> resultDataList = new List<ResultData>();

        public Form1()
        {
            InitializeComponent();
        }

        private void readTextBoxList(int size, string prefix, out double[] intArray)
        {
            intArray = new double[size];
            for (int i = 0; i < size; ++i)
            {
                foreach (Control contrl in this.Controls)
                {
                    int controlIndex = i + 1;
                    if (contrl.Name == (prefix + controlIndex.ToString()))
                    {
                        intArray[i] = Convert.ToDouble(contrl.Text);
                    }
                }
            }
        }

        private void onRateTextChanged(object sender, EventArgs e)
        {
            try
            {
                int totalNumber = Convert.ToInt32(numberBox.Text);

                double[] winBetPoints;
                readTextBoxList(totalNumber, "winBet", out winBetPoints);

                double[] placeBetPoints;
                readTextBoxList(totalNumber, "placeBet", out placeBetPoints);

                double totalWinBetPoint = 0;
                double totalPlaceBetPoint = 0;
                for (int i = 0; i < totalNumber; ++i)
                {
                    totalWinBetPoint += winBetPoints[i];
                    totalPlaceBetPoint += placeBetPoints[i];
                }

                double leftPoint = TOTAL_POINT - totalWinBetPoint - totalPlaceBetPoint;
                pointBox.Text = Convert.ToString(leftPoint);
            }
            catch
            {
                MessageBox.Show("Invalid number 1");
            }
        }

        private void calculateBtn_Click(object sender, EventArgs e)
        {
            try
            {
                resultBox.Clear();
                resultDataList.Clear();

                int totalNumber = Convert.ToInt32(numberBox.Text);
                int totalPlace = Convert.ToInt32(placesBox.Text);

                double[] winRates;
                readTextBoxList(totalNumber, "winRate", out winRates);

                double[] placeRates;
                readTextBoxList(totalNumber, "placeRate", out placeRates);

                double[] winBetPoints;
                readTextBoxList(totalNumber, "winBet", out winBetPoints);

                double[] placeBetPoints;
                readTextBoxList(totalNumber, "placeBet", out placeBetPoints);

                
                if (totalPlace == 2)
                {
                    for (int i = 0; i < totalNumber; ++i)
                    {
                        for (int j = 0; j < totalNumber; ++j)
                        {
                            if (i == j)
                                continue;
                            double winPoint = winRates[i] * winBetPoints[i] + placeRates[i] * placeBetPoints[i] + placeRates[j] * placeBetPoints[j];
                            resultDataList.Add(new ResultData(i+1, j+1, 2, winPoint));  
                        }
                    }
                }
                else if (totalPlace == 3)
                {
                    for (int i = 0; i < totalNumber; ++i)
                    {
                        for (int j = 0; j < totalNumber; ++j)
                        {
                            if (i == j)
                                continue;
                            for (int k = 0; k < totalNumber; ++k)
                            {
                                if (j == k || i == k)
                                    continue;
                                double winPoint = winRates[i] * winBetPoints[i] + placeRates[i] * placeBetPoints[i] 
                                    + placeRates[j] * placeBetPoints[j]
                                    + placeRates[k] * placeBetPoints[k];
                                resultDataList.Add(new ResultData(i + 1, j + 1, k+1, 3, winPoint));  
                            }
                        }
                    }
                }
                else
                {
                    MessageBox.Show("Places not supported");
                }

                bool printSeparator = false;
                resultDataList.Sort();
                foreach (ResultData resultData in resultDataList)
                {
                    if (!printSeparator && resultData.winPoint > TOTAL_POINT)
                    {
                        printSeparator = true;
                        resultBox.AppendText("-------------------\n");
                    }
                    resultBox.AppendText(resultData.ToString() + "\n");
                }
            }
            catch
            {
                MessageBox.Show("Invalid number 2");
            }
        }

    }

    public class ResultData :  IComparable<ResultData>
    {
        public int place1;
        public int place2;
        public int place3;

        public int totalPlace;
        public double winPoint;

        public ResultData(int place1_, int place2_, int totalPlace_, double winPoint_)
        {
            place1 = place1_;
            place2 = place2_;
            place3 = 0;
            totalPlace = totalPlace_;
            winPoint = winPoint_;
        }

        public ResultData(int place1_, int place2_, int place3_, int totalPlace_, double winPoint_)
        {
            place1 = place1_;
            place2 = place2_;
            place3 = place3_;
            totalPlace = totalPlace_;
            winPoint = winPoint_;
        }

        public override string ToString()
        {
            if (totalPlace == 2)
            {
                return String.Format("{0:N3} : {1:N2}-{2:N2}", winPoint, place1, place2);
            }
            else if (totalPlace == 3)
            {
                return String.Format("{0:N3} : {1:N2}-{2:N2}-{3:N2}", winPoint, place1, place2, place3);
            }

            return "NOT SUPPORTED PLACE";
        }

        public int CompareTo(ResultData result)
        {
            // A null value means that this object is greater.
            if (result == null)
                return 1;

            else
                return this.winPoint.CompareTo(result.winPoint);
        }
    }

}
