using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Analysis
{
    public partial class analysis : Form
    {
        private List<MarketData> marketDataList;
        private List<MarketData> marketDataList2;
        private List<MarketData> marketDataListRef1;
        private List<MarketData> marketDataListRef2;

        public analysis()
        {
            InitializeComponent();
            marketDataList = new List<MarketData>();
            marketDataList2 = new List<MarketData>();
            marketDataListRef1 = new List<MarketData>();
            marketDataListRef2 = new List<MarketData>();
        }

        enum LongShortEnum
        {
            ALL = 0,
            Long = 1,
            Short = 2
        };

        private void HighLowOpenStart_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = 0;
            double enterPert = 0;
            double stopPert = 0;

            double enterPert2 = 0;
            double stopPert2 = 0;
            // check parameters
            try
            {
                lastDays = Convert.ToInt32(HighLowOpen_lastDays.Text);
                enterPert = Convert.ToDouble(HighLowOpen_EnterPert.Text);
                stopPert = Convert.ToDouble(HighLowOpen_StopPert.Text);

                enterPert2 = Convert.ToDouble(HighLowOpen_EnterPert2.Text);
                stopPert2 = Convert.ToDouble(HighLowOpen_StopPert2.Text);
            }
            catch(Exception )
            {
                MessageBox.Show("Wrong parameter!");
                return;
            }
            
            Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();

            CalculateHighLowOpen(marketDataList, lastDays, enterPert, stopPert, false, ref result);

            if (marketDataList2.Count > 0)
            {
                CalculateHighLowOpen(marketDataList2, lastDays, enterPert2, stopPert2, false, ref result);
            }
            DisplayResult(result, 0, 0, true);
        }

        struct EnterStopPair
        {
            public double enter;
            public double stop;
            public EnterStopPair(double enter_, double stop_)
            {
                enter = enter_;
                stop = stop_;
            }
        };

        private void hiLowAnalysis_Click(object sender, EventArgs e)
        {
            bool displayInOrder = false;
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = Convert.ToInt32(HighLowOpen_lastDays.Text);

            double bestGain = 0;
            double bestEnter = 0;
            double bestStop = 0;

            SortedDictionary<double, EnterStopPair> resultMap = new  SortedDictionary<double, EnterStopPair>();
            for (double enterPert = 0.005; enterPert < 0.06; enterPert += 0.005)
            {
                for (double stopPert = 0.005; stopPert < 0.06; stopPert += 0.005)
                {
                    Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();
                    CalculateHighLowOpen(marketDataList, lastDays, enterPert, stopPert, false, ref result);
                    double gain = DisplayResult(result, enterPert, stopPert, false);

                    if (!resultMap.ContainsKey(gain))
                    {
                        resultMap.Add(gain, new EnterStopPair(enterPert, stopPert));
                    }


                    if (gain > bestGain)
                    {
                        bestGain = gain;
                        bestEnter = enterPert;
                        bestStop = stopPert;
                    }
                }
            }

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("Best: ({0:N3}:{1:N3}):gain={2:N4}\n", bestEnter, bestStop, bestGain));

            if (displayInOrder)
            {
                foreach (KeyValuePair<double, EnterStopPair> result in resultMap.Reverse())
                {
                    analysisResult.AppendText
                        (String.Format("({0:N3}:{1:N3}) average={2:N4}\n", result.Value.enter, result.Value.stop, result.Key));
                }
            }

        }

        private void CalculateHighLowOpen(List<MarketData> marketDataList, Int32 lastDays, double enterPert, double stopPert,
                                          bool fullLog, ref Dictionary<ResultKey, ResultData> result)
        {
            int startIndex = 0;
            if (lastDays > 0 && lastDays < marketDataList.Count)
            {
                startIndex = marketDataList.Count - lastDays;
            }

            for (int i = startIndex + 1; i < marketDataList.Count; ++i)
            {
                ResultKey key = new ResultKey(marketDataList[i].date.Year,marketDataList[i].date.Month);

                if (!result.ContainsKey(key))
                {
                    ResultData resultData = new ResultData();
                    resultData.totalLongGain = 1;
                    result.Add(key, resultData);
                }

                ResultData currMonthResult = result[key];

                // last day's index
                int prevDay = i - 1;

                double prevClose = marketDataList[prevDay].adj_close;
                double prevHigh = marketDataList[prevDay].high;

                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].adj_close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                /*
                // open high
                if (todayOpen / prevClose > (1 + enterPert))
                {
                    currMonthResult.longCount++;

                    if (todayLow / todayOpen > (1 - stopPert))
                    {
                        double gain = ((todayClose / todayOpen) - 1);
                        currMonthResult.totalLongGain += gain;

                        if (todayClose > todayOpen)
                        {
                            currMonthResult.longWin++;
                        }
                        else
                        {
                            currMonthResult.longLoss++;
                        }

                        if (fullLog)
                        {
                            analysisResult.AppendText
                                (String.Format("{0}: Gain={1:N4}\n", marketDataList[i].date.ToString(), gain));
                        }
                    }
                    else
                    {
                        currMonthResult.totalLongGain -= stopPert;
                        currMonthResult.longStop++;

                        if (fullLog)
                        {
                            analysisResult.AppendText
                                (String.Format("{0}: Gain={1:N4}\n", marketDataList[i].date.ToString(), -stopPert));
                        }
                    }
                }
       */
              

                if (todayHigh / prevHigh > (1 + enterPert))
                {
                    currMonthResult.longCount++;

                    double enterPrice = prevHigh * (1 + enterPert);

                    if (todayOpen > enterPrice)
                    {
                        enterPrice = todayOpen;
                    }

                    if (enterPrice > todayClose)
                    {
                        //double lossPert = (enterPrice - todayClose) / enterPrice;
                        double lossPert = stopPert;
                        currMonthResult.longLoss++;
                        currMonthResult.totalLongGain *= (1 - lossPert);
                    }
                    else
                    {
                        double gainPert = todayClose / enterPrice;
                        currMonthResult.totalLongGain *= gainPert;

                        currMonthResult.longWin++;
                    }
                }
                
            }
        }

        private void lastDayChangeStart_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            // check parameters
            Int32 lastDays = Convert.ToInt32(ld_lastDays.Text);
            double changePert = Convert.ToDouble(ld_changePert.Text);
            double stopPert = Convert.ToDouble(ld_stopPert.Text);
            LongShortEnum longShort = (LongShortEnum)Convert.ToUInt32(LastDayLongShort.Text);

            // clear result display
            analysisResult.Clear();

            Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();

            calcluateLastChangeResult(marketDataList, lastDays, changePert, stopPert, longShort, ref result);

            DisplayResult(result, 0, 0, true);
        }

        private void calcluateLastChangeResult(List<MarketData> marketDataList, Int32 lastDays, double changePert, double stopPert, LongShortEnum longShort, 
                                               ref Dictionary<ResultKey, ResultData> result)
        {
            int index;
            if (lastDays <= 0 || lastDays > marketDataList.Count - 3)
            {
                index = marketDataList.Count - 3;
            }
            else
            {
                index = lastDays;
            }

            for (int i = index; i >= 0; --i)
            {
                ResultKey key = new ResultKey(marketDataList[i].date.Year, marketDataList[i].date.Month);

                if (!result.ContainsKey(key))
                {
                    ResultData resultData = new ResultData();
                    result.Add(key, resultData);
                }

                ResultData currMonthResult = result[key];

                // last day's index
                int prevDay = i + 1;
                int prev2Day = i + 2;

                double prevOpen = marketDataList[prevDay].open;
                double prevClose = marketDataList[prevDay].close;
                double prev2Close = marketDataList[prev2Day].close;
                double prevClosePert = prevClose / prev2Close;

                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                if ((longShort == LongShortEnum.ALL || longShort == LongShortEnum.Long) && 
                    prevClosePert >= (1 + changePert))
                {
                   // if (todayOpen > prevClose)
                    {
                        currMonthResult.longCount++;

                        if (todayLow / todayOpen > (1 - stopPert))
                        {
                            currMonthResult.totalLongGain += ((todayClose / todayOpen) - 1);

                            if (todayClose > todayOpen)
                            {
                                currMonthResult.longWin++;
                            }
                            else
                            {
                                currMonthResult.longLoss++;
                            }
                        }
                        else
                        {
                            currMonthResult.totalLongGain -= stopPert;
                            currMonthResult.longStop++;
                        }
                    }
                }
                else if ((longShort == LongShortEnum.ALL || longShort == LongShortEnum.Short) && 
                         prevClosePert <= (1 - changePert))
                {
                  //  if (todayOpen < prevClose)
                    {
                        currMonthResult.shortCount++;

                        if (todayHigh / todayOpen < (1 + stopPert))
                        {
                            currMonthResult.totalShortGain += ((todayOpen / todayClose) - 1);

                            if (todayClose < todayOpen)
                            {
                                currMonthResult.shortWin++;
                            }
                            else
                            {
                                currMonthResult.shortLoss++;
                            }
                        }
                        else
                        {
                            currMonthResult.totalShortGain -= stopPert;
                            currMonthResult.shortStop++;
                        }
                    }
                }
            }
        }

        private void lastDayAnalysis_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = Convert.ToInt32(ld_lastDays.Text);
            LongShortEnum longShort = (LongShortEnum)Convert.ToUInt32(LastDayLongShort.Text);

            double bestGain = 0;
            double bestLastDayPert = 0;
            double bestStop = 0;

            for (double lastDayPert = 0.01; lastDayPert < 0.04; lastDayPert += 0.001)
            {
                for (double stopPert = 0.01; stopPert < 0.04; stopPert += 0.001)
                {
                    Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();
                    calcluateLastChangeResult(marketDataList, lastDays, lastDayPert, stopPert, longShort, ref result);
                    double gain = DisplayResult(result, lastDayPert, stopPert, false);

                    if (gain > bestGain)
                    {
                        bestGain = gain;
                        bestLastDayPert = lastDayPert;
                        bestStop = stopPert;
                    }
                }
            }

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("Best: ({0:N3}:{1:N3}):gain={2:N4}\n", bestLastDayPert, bestStop, bestGain));
        }

        private double DisplayResult(Dictionary<ResultKey, ResultData> result, double enterPert, double stopPert, bool fullLog)
        {
            int totalLongCount = 0;
            int totalLongWin = 0;
            int totalLongLoss = 0;

            double worstMonthlyGain = 100;
            double bestMonthlyGain = -100;
            double totalGain = 1;

            int year = 0;
            double yearGain = 0;

            foreach (KeyValuePair<ResultKey, ResultData> monthResult in result)
            {
                if (year != monthResult.Key.year)
                {
                    if (year != 0)
                    {
                        if (fullLog)
                        {
                            analysisResult.AppendText(String.Format("YEAR={0}, GAIN={1:N4}\n", year, (double)yearGain));
                        }
                    }

                    year = monthResult.Key.year;
                    yearGain = 1;
                }

                totalLongCount += monthResult.Value.longCount;
                totalLongWin += monthResult.Value.longWin;
                totalLongLoss += monthResult.Value.longLoss;


                // calculate open high/low gain
                double curnMonthGain = monthResult.Value.totalLongGain;

                if (worstMonthlyGain > curnMonthGain)
                {
                    worstMonthlyGain = curnMonthGain;
                }
                if (bestMonthlyGain < curnMonthGain)
                {
                    bestMonthlyGain = curnMonthGain;
                }

                yearGain *= curnMonthGain;
                totalGain *= curnMonthGain;

                if (fullLog)
                {
                    analysisResult.AppendText
                        (String.Format("{0}:High O/L={1:00},Win={2:00},Gain={3:N4}\n", monthResult.Key.toString(), monthResult.Value.longCount,
                        monthResult.Value.longWin, curnMonthGain));
                }
            }

            if (fullLog)
            {
                analysisResult.AppendText(String.Format("YEAR={0}, GAIN={1:N4}\n", year, yearGain));
            }


            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("({0:N3}:{1:N3}):Worst={2:N4}, Best={3:N4}, average={4:N4}\n", enterPert, stopPert, worstMonthlyGain, bestMonthlyGain, totalGain));

            if (fullLog)
            {
                analysisResult.AppendText("--------------------\n");
                analysisResult.AppendText
                    (String.Format("Open high:{0}, Close high:{1}, percentage:{2:N4}, Loss:{3}\n", totalLongCount, totalLongWin, (double)totalLongWin / totalLongCount, totalLongLoss));
            }

            return totalGain;
        }

        private double DisplayResultBuyHigh(Dictionary<ResultKey, ResultData> result, double buyPert, double enterPert, double stopPert, bool fullLog)
        {
            int totalLongCount = 0;
            int totalLongWin = 0;
            int totalLongLoss = 0;

            int totalShortCount = 0;
            int totalShortWin = 0;

            double worstMonthlyGain = 100;
            double bestMonthlyGain = -100;
            double avgMonthlyGain = 0;

            int year = 0;
            int yearMonth = 0;
            double yearGain = 0;

            foreach (KeyValuePair<ResultKey, ResultData> monthResult in result)
            {
                if (year != monthResult.Key.year)
                {
                    if (year != 0)
                    {
                        if (fullLog)
                        {
                            analysisResult.AppendText(String.Format("YEAR={0}, GAIN={1:N4}\n", year, (double)yearGain / yearMonth));
                        }
                    }

                    year = monthResult.Key.year;
                    yearMonth = 0;
                    yearGain = 0;
                }

                yearMonth++;

                totalLongCount += monthResult.Value.longCount;
                totalLongWin += monthResult.Value.longWin;
                totalLongLoss += monthResult.Value.longLoss;

                totalShortCount += monthResult.Value.shortCount;
                totalShortWin += monthResult.Value.shortWin;

                // calculate open high/low gain
                double totalMonthlyGain = monthResult.Value.totalLongGain + monthResult.Value.totalShortGain;

                if (worstMonthlyGain > totalMonthlyGain)
                {
                    worstMonthlyGain = totalMonthlyGain;
                }
                if (bestMonthlyGain < totalMonthlyGain)
                {
                    bestMonthlyGain = totalMonthlyGain;
                }
                avgMonthlyGain += totalMonthlyGain;

                yearGain += totalMonthlyGain;

                if (fullLog)
                {
                    analysisResult.AppendText
                        (String.Format("{0}:High O/L={1:00},Win={2:00},Gain={3:N4}\n", monthResult.Key.toString(), (monthResult.Value.longCount + monthResult.Value.shortCount),
                        (monthResult.Value.longWin + monthResult.Value.shortWin), totalMonthlyGain));
                }
            }

            if (fullLog)
            {
                analysisResult.AppendText(String.Format("YEAR={0}, GAIN={1:N4}\n", year, (double)yearGain / yearMonth));
            }

            avgMonthlyGain = avgMonthlyGain / result.Count;

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("({0:N3}:{1:N3}:{2:N3}):Worst={3:N4}, Best={4:N4}, average={5:N4}\n", buyPert, enterPert, stopPert, worstMonthlyGain, bestMonthlyGain, avgMonthlyGain));

            if (fullLog)
            {
                analysisResult.AppendText("--------------------\n");
                analysisResult.AppendText
                    (String.Format("Open high:{0}, Close high:{1}, percentage:{2:N4}, Loss:{3}\n", totalLongCount, totalLongWin, (double)totalLongWin / totalLongCount, totalLongLoss));
                analysisResult.AppendText
                    (String.Format("Open Low:{0}, Close Low:{1}, percentage:{2:N4}\n", totalShortCount, totalShortWin, (double)totalShortWin / totalShortCount));
            }

            return avgMonthlyGain;
        }

        private double CountOpenHighEnterRange(List<MarketData> marketDataList, Int32 lastDays, double enterPertLow, double enterPertHigh, double stopPert)
        {
            int index;
            if (lastDays <= 0 || lastDays > marketDataList.Count - 2)
            {
                index = marketDataList.Count - 2;
            }
            else
            {
                index = lastDays;
            }

            int longCount = 0;
            int longWin = 0;
            double avgProfit = 0;

            analysisResult.AppendText
                  (String.Format("Date  OpenChange  GLD  UUP  CloseChange\n"));

            for (int i = index; i >= 0; --i)
            {
                // last day's index
                int prevDay = i + 1;

                double prevCloseRef1 = marketDataListRef1[prevDay].close;
                double todayOpenRef1 = marketDataListRef1[i].open;

                double prevCloseRef2 = marketDataListRef2[prevDay].close;
                double todayOpenRef2 = marketDataListRef2[i].open;

                double prevClose = marketDataList[prevDay].close;
                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                if (todayOpen / prevClose > (1 + enterPertLow) && todayOpen / prevClose < (1 + enterPertHigh))
                {
                    double ref1Change = todayOpenRef1 / prevCloseRef1;
                    double ref2Change = todayOpenRef2 / prevCloseRef2;
                    double openChange = todayOpen / prevClose;
                    double todayChange = todayClose / todayOpen;
                    int isWin = 0;

                    longCount++;

                    if (todayLow / todayOpen > (1 - stopPert))
                    {
                        double gain = ((todayClose / todayOpen) - 1);
                        avgProfit += gain;

                        if (todayClose > todayOpen)
                        {
                            longWin++;

                            isWin = 1;
                        }
                    }
                    else
                    {
                        avgProfit -= (stopPert + 0.002);
                    }

                    if (isWin==1 && todayChange > 1.05)
                    {
                        analysisResult.AppendText
                            (String.Format("{0} : {1:N3}, {2:N3}, {3:N3}, {4:N3}, {5} \n",
                            marketDataList[i].date.ToShortDateString(), openChange, ref1Change, ref2Change, todayChange, isWin));
                    }
                }
            }

            if (longCount > 0)
            {
                avgProfit = avgProfit / longCount;
            }
            
            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("({0:N3}-{1:N3}:{2:N3}): Tradedays={3:N4}, Windays={4:N5} WinPert={5:N4}, profit={6:N5}\n",
                enterPertLow, enterPertHigh, stopPert, longCount, longWin, (double)longWin / longCount, avgProfit));

            return avgProfit;
        }

        private void CalculatePertCrossover(List<MarketData> marketDataList, Int32 lastDays, double enterPert, double stopPert,
                                          ref List<TradeData> result)
        {
            int index;
            if (lastDays <= 0 || lastDays > marketDataList.Count - 2)
            {
                index = marketDataList.Count - 2;
            }
            else
            {
                index = lastDays;
            }

            TradeState state = TradeState.TRADE_WAIT;
            TradeData trade = new TradeData();

            for (int i = index; i >= 0; --i)
            {
                // last day's index
                int prevDay = i + 1;

                double prevOpen = marketDataList[prevDay].open;
                double prevHigh = marketDataList[prevDay].high;
                double prevClose = marketDataList[prevDay].close;
                double prevLow = marketDataList[prevDay].low;

                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                if (state == TradeState.TRADE_WAIT)
                {
                    // looking for buy signal
                    double buyPrice = prevHigh * (1 + enterPert);

                    if (todayHigh > buyPrice)
                    {
                        if (todayOpen > buyPrice)
                        {
                            buyPrice = todayOpen;
                        }
                        state = TradeState.TRADE_BUYIN;
                        trade.buyTime = marketDataList[i].date;
                        trade.buyPrice = buyPrice;   

                        double stopPrice = buyPrice * (1 - stopPert);
                        if (todayOpen > prevClose && todayLow < stopPrice)
                        {
                            state = TradeState.TRADE_WAIT;
                            trade.sellTime = marketDataList[i].date;
                            trade.sellPrice = stopPrice;

                            result.Add(new TradeData(trade));
                            trade.clear();
                        }

                    }
                }
                
                else if (state == TradeState.TRADE_BUYIN)
                {
                    // looking for sell signal
                    double sellPrice = prevLow * (1 - stopPert);

                    if (todayLow < sellPrice)
                    {
                        if (todayOpen < sellPrice)
                        {
                            sellPrice = todayOpen;
                        }

                        state = TradeState.TRADE_WAIT;
                        trade.sellTime = marketDataList[i].date;
                        trade.sellPrice = sellPrice;

                        result.Add(new TradeData(trade));
                        trade.clear();
                    }
                }

            }
        }

        private double DisplayTradeDataResult(List<TradeData> result, double enterPert, double stopPert, bool fullLog)
        {
            double totalProfit = 0;
            int totalTrade = 0;
            int totalDays = 0;
            foreach (TradeData trade in result)
            {
                totalTrade++;

                double profit = (trade.sellPrice / trade.buyPrice -1);
                TimeSpan diffDate = trade.sellTime.Subtract(trade.buyTime);
                int tradeDays = diffDate.Days;

                totalProfit += profit;
                totalDays += tradeDays;

                if (fullLog)
                {
                    analysisResult.AppendText
                        (String.Format("[{0}]-[{1}] Buy={2:N2}, Sell={3:N2}, profite={4:N4}, Days={5}\n",
                        trade.buyTime.ToShortDateString(), trade.sellTime.ToShortDateString(), trade.buyPrice, trade.sellPrice, profit, tradeDays));
                }
            }

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("{0:N3}:{1:N3}):Profit={2:N4}, Trade={3:N2}, Trade days={4:N4}\n", enterPert, stopPert, totalProfit, totalTrade, totalDays));
            return totalProfit;
        }
        

        private void analysis_Load(object sender, EventArgs e)
        {

        }

        private void pertCrossOverAnalysis_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = 0;
            double enterPertLow = 0;
            double enterPertHigh = 0;

            try
            {
                lastDays = Convert.ToInt32(buyStopLastDays.Text);
                enterPertLow = Convert.ToDouble(buyStop_enterPert.Text);
                enterPertHigh = Convert.ToDouble(buyStop_stopPert.Text);

            }
            catch (Exception)
            {
                MessageBox.Show("Wrong parameter!");
                return;
            }

            double bestGain = 0;
            double bestStop = 0;

            for (double stopPert = 0.005; stopPert < 0.04; stopPert += 0.001)
            {
                double gain = CountOpenHighEnterRange(marketDataList, lastDays, enterPertLow, enterPertHigh, stopPert);

                if (gain > bestGain)
                {
                    bestGain = gain;
                    bestStop = stopPert;
                }
            }

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("Best: ({0:N3}):gain={1:N4}\n", bestStop, bestGain));
        }

        private void BuyHighCalculate(List<MarketData> marketDataList, Int32 lastDays, double openPert, double enterPert, double stopPert,
                                      ref Dictionary<ResultKey, ResultData> result)
        {
            int index;
            if (lastDays <= 0 || lastDays > marketDataList.Count - 2)
            {
                index = marketDataList.Count - 2;
            }
            else
            {
                index = lastDays;
            }

            for (int i = index; i >= 0; --i)
            {
                ResultKey key = new ResultKey(marketDataList[i].date.Year, marketDataList[i].date.Month);

                if (!result.ContainsKey(key))
                {
                    ResultData resultData = new ResultData();
                    result.Add(key, resultData);
                }

                ResultData currMonthResult = result[key];

                // last day's index
                int prevDay = i + 1;

                double prevClose = marketDataList[prevDay].close;

                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                // open 
                //if ((todayOpen / prevClose > (1 - openPert)) && (todayOpen / prevClose < (1 + openPert)))
                if (todayOpen > prevClose && (todayOpen / prevClose < (1 + openPert)))
                {
                    double highPricePert = todayHigh / prevClose -1;
                    double lowPricePert = 1- (todayLow / prevClose);
                   
                    
                    if (highPricePert > enterPert)
                    {
                        currMonthResult.longCount++;

                        double buyPrice = prevClose * (1 + enterPert);
                        double stopPrice = buyPrice * (1 + stopPert);

                        // trigger stop order
                        if (todayHigh > stopPrice)
                        {
                            currMonthResult.longLoss++;
                            currMonthResult.totalLongGain -= (stopPert + 0.002);
                        }
                        else
                        {
                            currMonthResult.longWin++;
                            currMonthResult.totalLongGain += (buyPrice/todayClose - 1);
                        }

                    }
                   /* else if (lowPricePert > enterPert)
                    {
                        currMonthResult.longCount++;

                        double buyPrice = prevClose * (1 - enterPert);
                        double stopPrice = buyPrice * (1 - stopPert);

                        // trigger stop order
                        if (todayLow < stopPrice)
                        {
                            currMonthResult.longLoss++;
                            currMonthResult.totalLongGain -= (stopPert + 0.002);
                        }
                        else
                        {
                            currMonthResult.longWin++;
                            currMonthResult.totalLongGain += (todayClose / buyPrice - 1);
                        }
                    }    */             
                }
               
            }
        }

        private void OpenHighAnalysis_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = Convert.ToInt32(buyHigh_LastDays.Text);

            double bestGain = 0;
            double bestBuy = 0;
            double bestEnter = 0;
            double bestStop = 0;

           // for (double buyPert = 0.03; buyPert > 0.003; buyPert -= 0.002)
            {
                double buyPert = 0.026;
                for (double enterPert = 0.04; enterPert < 0.10; enterPert += 0.002)
                {
                    for (double stopPert = 0.01; stopPert < 0.05; stopPert += 0.002)
                    {
                        Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();
                        BuyHighCalculate(marketDataList, lastDays, buyPert, enterPert, stopPert, ref result);
                        double gain = DisplayResultBuyHigh(result, buyPert, enterPert, stopPert, false);
                        if (gain > bestGain)
                        {
                            bestGain = gain;
                            bestBuy = buyPert;
                            bestEnter = enterPert;
                            bestStop = stopPert;
                        }

                    }
                }
            }

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("Best: ({0:N3}:{1:N3}:{2:N3}) gain={3:N4}\n", bestBuy, bestEnter, bestStop, bestGain));
        }

        private void OpenHighSellLimit_Start_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = 0;
            double buyPert = 0;
            double enterPert = 0;
            double stopPert = 0;

            // check parameters
            try
            {
                lastDays = Convert.ToInt32(buyHigh_LastDays.Text);
                buyPert = Convert.ToDouble(buyHigh_OpenPert.Text);
                enterPert = Convert.ToDouble(buyHigh_EnterPert.Text);
                stopPert = Convert.ToDouble(buyHigh_StopPert.Text);
            }
            catch (Exception)
            {
                MessageBox.Show("Wrong parameter!");
                return;
            }

            Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();

            BuyHighCalculate(marketDataList, lastDays, buyPert, enterPert, stopPert, ref result);

            DisplayResultBuyHigh(result, 0, 0, 0, true);
        }

        private void clickLoad(object sender, EventArgs e)
        {
            loadPriceFile(ref marketDataList);
        }

        private void load2ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            loadPriceFile(ref marketDataList2);
        }

        private void unload2ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            marketDataList2.Clear();
            fileName2.Text = "";
        }

        private void loadRef1_Click(object sender, EventArgs e)
        {
            loadPriceFile(ref marketDataListRef1);
        }

        private void loadRef2_Click(object sender, EventArgs e)
        {
            loadPriceFile(ref marketDataListRef2);
        }

        private void loadPriceFile(ref List<MarketData> marketDataList)
        {
            // Create an instance of the open file dialog box.
            OpenFileDialog openFileDialog = new OpenFileDialog();

            // Call the ShowDialog method to show the dialog box.
            DialogResult result = openFileDialog.ShowDialog();


            if (result == DialogResult.OK)
            {
                marketDataList.Clear();

                // Open the selected file to read.
                System.IO.Stream fileStream = openFileDialog.OpenFile();
                fileName1.Text = openFileDialog.FileName;

                using (System.IO.StreamReader reader = new System.IO.StreamReader(fileStream))
                {
                    // Read the first line from the file and write it the textbox.
                    while (!reader.EndOfStream)
                    {
                        string fileStr = reader.ReadLine();
                        string[] tokens = fileStr.Split(',');
                        if (tokens[0].Equals("Date", StringComparison.OrdinalIgnoreCase))
                        {
                            continue;
                        }
                        if (tokens.Length < 6)
                        {
                            continue;
                        }

                        MarketData marketData = new MarketData();
                        marketData.date = Convert.ToDateTime(tokens[0]);
                        marketData.open = Convert.ToDouble(tokens[1]);
                        marketData.high = Convert.ToDouble(tokens[2]);
                        marketData.low = Convert.ToDouble(tokens[3]);
                        marketData.close = Convert.ToDouble(tokens[4]);
                        marketData.adj_close = Convert.ToDouble(tokens[5]);
                        marketData.volume = Convert.ToDouble(tokens[6]);

                        marketDataList.Add(marketData);
                    }
                }

                fileStream.Close();
            }
        }

        private void highLow_Display_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = 0;
            double enterPert = 0;
            double stopPert = 0;
            double enterPert2 = 0;
            double stopPert2 = 0;
            // check parameters
            try
            {
                lastDays = Convert.ToInt32(HighLowOpen_lastDays.Text);
                enterPert = Convert.ToDouble(HighLowOpen_EnterPert.Text);
                stopPert = Convert.ToDouble(HighLowOpen_StopPert.Text);

                enterPert2 = Convert.ToDouble(HighLowOpen_EnterPert2.Text);
                stopPert2 = Convert.ToDouble(HighLowOpen_StopPert2.Text);
            }
            catch (Exception)
            {
                MessageBox.Show("Wrong parameter!");
                return;
            }

            int index;
            if (lastDays <= 0 || lastDays > marketDataList.Count - 2)
            {
                index = marketDataList.Count - 2;
            }
            else
            {
                index = lastDays;
            }

            double totalPert = 0;
            int count = 0;

            for (int i = index; i >= 0; --i)
            {
                ResultKey key = new ResultKey(marketDataList[i].date.Year, marketDataList[i].date.Month);

                // last day's index
                int prevDay = i + 1;

                double prevClose = marketDataList[prevDay].close;

                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                
                // open high
                if (todayOpen / prevClose > (1 + enterPert))
                {
                    if (todayLow / todayOpen > (1 - stopPert))
                    {
                        continue;
                    }
                    continue;
                }
                // open low
                else if (todayOpen / prevClose < (1 - enterPert2))
                {
                    if (todayHigh / todayOpen < (1 + stopPert2))
                    {
                        continue;
                    }
                    continue;
                }

                count++;
                double highPert = todayHigh / prevClose - 1;
                double lowPert = 1 - todayLow / prevClose;
                double pert = highPert > lowPert ? highPert : lowPert;

                totalPert += pert;
                analysisResult.AppendText(String.Format("{0:N3}\n", pert));
            }
            analysisResult.AppendText("----------------\n");
            analysisResult.AppendText(String.Format("average = {0:N3}\n", totalPert/count));
        }

        private void longInvest_Start_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = 0;
            double buyPert = 0;
            double stopPert = 0;

            // check parameters
            try
            {
                lastDays = Convert.ToInt32(longInvest_days.Text);
                buyPert = Convert.ToDouble(longInvest_buyPert.Text);
                stopPert = Convert.ToDouble(longInvest_stopPert.Text);
            }
            catch (Exception)
            {
                MessageBox.Show("Wrong parameter!");
                return;
            }

            List<LongResultData> longResultDataList = new List<LongResultData>();

            CalculateLongInvest(marketDataList, lastDays, buyPert, stopPert, ref longResultDataList);

            DisplayResultLongInvest(longResultDataList, 0, 0, true);
        }

        private double DisplayResultLongInvest(List<LongResultData> resultList, double buyPert, double sellPert, bool fullLog)
        {
            double totalProfit = 1;
            for (int i = 0; i < resultList.Count; ++i)
            {
                totalProfit = totalProfit * resultList[i].profit();

                if (fullLog)
                {
                    analysisResult.AppendText(String.Format("{0}-{1}, buy={2:N2}, sell={3:N2}, profit={4:N2}\n",
                        resultList[i].buyDate.ToShortDateString(), resultList[i].sellDate.ToShortDateString(), resultList[i].buyPrice, resultList[i].sellPrice, resultList[i].profit()));
                }
            }
            analysisResult.AppendText("------------------------\n");
            analysisResult.AppendText(String.Format("{0:N3}:{1:N3},TotalProfit={2:N2}\n", buyPert, sellPert, totalProfit));

            return totalProfit;
        }

        private void CalculateLongInvest(List<MarketData> marketDataList, Int32 lastDays, double buyPert, double stopPert,
                                         ref List<LongResultData> resultList)
        {
            int startIndex = 0;
            if (lastDays > 0 && lastDays < marketDataList.Count)
            {
                startIndex = marketDataList.Count - lastDays;
            }

            double lowPrice = marketDataList[startIndex].adj_close;
            double highPrice = 0;

            bool isBought = false;
            LongResultData longResultData = new LongResultData();

            for (int i = startIndex + 1; i < marketDataList.Count; ++i)
            {
                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].adj_close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                double high = todayClose > todayOpen ? todayClose : todayOpen;
                double low = todayClose > todayOpen ? todayOpen : todayClose;

                if (isBought)
                {
                    //double expSellPrice = marketDataList[i-1].adj_close * (1 - stopPert);
                    double expSellPrice = highPrice * (1 - stopPert);
                    if (todayLow <= expSellPrice)
                    {
                        longResultData.sellPrice = expSellPrice < todayOpen ? expSellPrice : todayOpen;
                        longResultData.sellDate = marketDataList[i].date;
                        isBought = false;
                        lowPrice = low;

                        resultList.Add(new LongResultData(longResultData));
                        longResultData.clear();
                    }
                    else
                    {
                        highPrice = highPrice > high ? highPrice : high;
                    }
                }
                else
                {
                    double expBuyPrice = lowPrice * (1 + buyPert);
                    if (todayHigh >= expBuyPrice)
                    {
                        isBought = true;
                        longResultData.buyPrice = expBuyPrice > todayOpen ? expBuyPrice : todayOpen;
                        longResultData.buyDate = marketDataList[i].date;
                        highPrice = high;
                    }
                    else
                    {

                        lowPrice = lowPrice < low ? lowPrice : low;
                    }
                }
            }
        }

        private void longInvest_Analysis_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = Convert.ToInt32(longInvest_days.Text);

            double bestGain = 0;
            double bestBuyPert = 0;
            double bestSellPert = 0;

            for (double enterPert = 0.01; enterPert <= 0.1; enterPert += 0.01)
            {
                for (double stopPert = 0.01; stopPert <= 0.1; stopPert += 0.01)
                {
                    List<LongResultData> longResultDataList = new List<LongResultData>();

                    CalculateLongInvest(marketDataList, lastDays, enterPert, stopPert, ref longResultDataList);
                    double gain = DisplayResultLongInvest(longResultDataList, enterPert, stopPert, false);

                    if (gain > bestGain)
                    {
                        bestGain = gain;
                        bestBuyPert = enterPert;
                        bestSellPert = stopPert;
                    }
                }
            }

            analysisResult.AppendText("--------------------\n");
            analysisResult.AppendText
                (String.Format("Best: ({0:N3}:{1:N3}):gain={2:N4}\n", bestBuyPert, bestSellPert, bestGain));

        }

        private void SellHighStart_Click(object sender, EventArgs e)
        {
            if (marketDataList.Count == 0)
            {
                MessageBox.Show("Load file first!");
                return;
            }
            analysisResult.Clear();

            Int32 lastDays = 0;
            double buyStopEnter = 0;
            double buyStopStopPert = 0;

            try
            {
                lastDays = Convert.ToInt32(buyStopLastDays.Text);
                buyStopEnter = Convert.ToDouble(buyStop_enterPert.Text);
                buyStopStopPert = Convert.ToDouble(buyStop_stopPert.Text);

            }
            catch (Exception)
            {
                MessageBox.Show("Wrong parameter!");
                return;
            }

            Dictionary<ResultKey, ResultData> result = new Dictionary<ResultKey, ResultData>();

            CalculateBuyStop(marketDataList, lastDays, buyStopEnter, buyStopStopPert, false, ref result);

            DisplayResult(result, 0, 0, true);

        }

        private void CalculateBuyStop(List<MarketData> marketDataList, Int32 lastDays, double enterPert, double sellPert,
                                          bool fullLog, ref Dictionary<ResultKey, ResultData> result)
        {
            int startIndex = 0;
            if (lastDays > 0 && lastDays < marketDataList.Count)
            {
                startIndex = marketDataList.Count - lastDays;
            }

            for (int i = startIndex + 1; i < marketDataList.Count; ++i)
            {
                ResultKey key = new ResultKey(marketDataList[i].date.Year, marketDataList[i].date.Month);

                if (!result.ContainsKey(key))
                {
                    ResultData resultData = new ResultData();
                    result.Add(key, resultData);
                }

                ResultData currMonthResult = result[key];

                // last day's index
                int prevDay = i - 1;

                double prevClose = marketDataList[prevDay].adj_close;

                double todayOpen = marketDataList[i].open;
                double todayClose = marketDataList[i].adj_close;
                double todayLow = marketDataList[i].low;
                double todayHigh = marketDataList[i].high;

                // open high
                if (todayOpen / prevClose > (1 + enterPert))
                {
                    currMonthResult.longCount++;

                    if (todayHigh / todayOpen > (1 + sellPert))
                    {
                        currMonthResult.totalLongGain += sellPert;
                        currMonthResult.longWin++;
                    }
                    else
                    {
                        double gain = (todayClose / todayOpen) - 1;
                        currMonthResult.totalLongGain += gain;
                        currMonthResult.longLoss++;
                    }
                }
            }
        }
    }

    enum TradeState
    {
        TRADE_WAIT = 0,
        TRADE_BUYIN = 1
    };

    public class TradeData
    {
        public DateTime buyTime;
        public DateTime sellTime;
        public double buyPrice;
        public double sellPrice;

        public TradeData() 
        {
            clear();
        }

        public TradeData(TradeData data)
        {
            buyTime = data.buyTime;
            sellTime = data.sellTime;
            buyPrice = data.buyPrice;
            sellPrice = data.sellPrice;
        }

        public void clear()
        {
            buyPrice = 0;
            sellPrice = 0;
        }
    };

    public class MarketData
    {
        public DateTime date;
        public double open;
        public double high;
        public double low;
        public double close;
        public double volume;
        public double adj_close;

        public MarketData()
        {
            clear();
        }

        public void clear()
        {
            open = 0;
            high = 0;
            low = 0;
            close = 0;
            volume = 0;
            adj_close = 0;
        }

    };

    public struct ResultKey
    {
        public int year;
        public int month;

        public ResultKey(int year_, int month_)
        {
            year = year_;
            month = month_;
        }

        public string toString()
        {
            return String.Format("{0}-{1:00}", year, month).ToString();
        }
    };

    public class ResultData
    {
        public Int32 longCount;
        public Int32 longWin;
        public Int32 longLoss;
        public Int32 longStop;
        public double totalLongGain;

        public Int32 shortCount;
        public Int32 shortWin;
        public Int32 shortLoss;
        public Int32 shortStop;
        public double totalShortGain;

        public ResultData()
        {
            clear();
        }

        public ResultData(ResultData result)
        {
            longCount = result.longCount;
            longWin = result.longWin;
            longLoss = result.longLoss;
            longStop = result.longStop;
            totalLongGain = result.totalLongGain;

            shortCount = result.shortCount;
            shortWin = result.shortWin;
            shortLoss = result.shortLoss;
            shortStop = result.shortStop;
            totalShortGain = result.totalShortGain;
        }

        public void clear()
        {
            longCount = 0;
            longWin = 0;
            longLoss = 0;
            longStop = 0;
            totalLongGain = 0;

            shortCount = 0;
            shortWin = 0;
            shortLoss = 0;
            shortStop = 0;
            totalShortGain = 0;
        }
    };

    public class LongResultData
    {
        public double buyPrice;
        public double sellPrice;
        public DateTime buyDate;
        public DateTime sellDate;

        public LongResultData(LongResultData data)
        {
            buyPrice = data.buyPrice;
            sellPrice = data.sellPrice;
            buyDate = data.buyDate;
            sellDate = data.sellDate;
        }

        public LongResultData()
        {
            clear();
        }

        public void clear()
        {
            buyPrice = 0;
            sellPrice = 0;
        }

        public double profit()
        {
            return sellPrice / buyPrice;
        }
    };
}
