//+------------------------------------------------------------------+
//|                                                 BreakoutStrategy.mq5 |
//|                                  Copyright 2024, Trading Strategies   |
//+------------------------------------------------------------------+
#property copyright "Copyright 2024, Trading Strategies"
#property link      "https://www.yourwebsite.com"
#property version   "1.00"
#property strict

// Input Parameters
input int      BB_Period = 20;           // Bollinger Bands Period
input double   BB_Deviation = 2;         // Bollinger Bands Standard Deviations
input int      RSI_Period = 14;          // RSI Period
input double   Volume_Multiplier = 1.5;   // Volume Breakout Multiplier
input double   Risk_Percent = 1.5;       // Risk Percent per Trade
input int      Lookback_Bars = 10;       // Bars to look back for S/R

// Handle Declarations
int BB_Handle;
int RSI_Handle;
int Volume_MA_Handle;

// Global Variables
double Upper_Band[], Middle_Band[], Lower_Band[];
double RSI[];
double Volume_MA[];
datetime LastTradeTime;

//+------------------------------------------------------------------+
//| Expert initialization function                                     |
//+------------------------------------------------------------------+
int OnInit()
{
    // Initialize indicator handles
    BB_Handle = iBands(_Symbol, PERIOD_CURRENT, BB_Period, 0, BB_Deviation, PRICE_CLOSE);
    RSI_Handle = iRSI(_Symbol, PERIOD_CURRENT, RSI_Period, PRICE_CLOSE);
    Volume_MA_Handle = iMA(_Symbol, PERIOD_CURRENT, BB_Period, 0, MODE_SMA, VOLUME_REAL);
    
    // Allocate arrays
    ArraySetAsSeries(Upper_Band, true);
    ArraySetAsSeries(Middle_Band, true);
    ArraySetAsSeries(Lower_Band, true);
    ArraySetAsSeries(RSI, true);
    ArraySetAsSeries(Volume_MA, true);
    
    return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                   |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
    // Clean up arrays
    ArrayFree(Upper_Band);
    ArrayFree(Middle_Band);
    ArrayFree(Lower_Band);
    ArrayFree(RSI);
    ArrayFree(Volume_MA);
}

//+------------------------------------------------------------------+
//| Expert tick function                                              |
//+------------------------------------------------------------------+
void OnTick()
{
    // Update indicator values
    CopyBuffer(BB_Handle, 0, 0, 3, Middle_Band);
    CopyBuffer(BB_Handle, 1, 0, 3, Upper_Band);
    CopyBuffer(BB_Handle, 2, 0, 3, Lower_Band);
    CopyBuffer(RSI_Handle, 0, 0, 3, RSI);
    CopyBuffer(Volume_MA_Handle, 0, 0, 3, Volume_MA);
    
    // Check for open positions
    if(PositionsTotal() > 0) return;
    
    // Check entry conditions
    if(IsLongBreakout())
    {
        OpenLongPosition();
    }
    else if(IsShortBreakout())
    {
        OpenShortPosition();
    }
}

//+------------------------------------------------------------------+
//| Check for long breakout conditions                                |
//+------------------------------------------------------------------+
bool IsLongBreakout()
{
    double currentClose = iClose(_Symbol, PERIOD_CURRENT, 0);
    double currentVolume = iVolume(_Symbol, PERIOD_CURRENT, 0);
    
    // Price above upper band
    bool priceBreakout = currentClose > Upper_Band[1];
    
    // Volume breakout
    bool volumeBreakout = currentVolume > Volume_MA[1] * Volume_Multiplier;
    
    // RSI not overbought
    bool rsiOK = RSI[1] < 70;
    
    // No recent higher high
    bool noRecentHigh = true;
    double currentHigh = iHigh(_Symbol, PERIOD_CURRENT, 0);
    
    for(int i = 1; i <= Lookback_Bars; i++)
    {
        if(iHigh(_Symbol, PERIOD_CURRENT, i) > currentHigh)
        {
            noRecentHigh = false;
            break;
        }
    }
    
    return priceBreakout && volumeBreakout && rsiOK && noRecentHigh;
}

//+------------------------------------------------------------------+
//| Check for short breakout conditions                               |
//+------------------------------------------------------------------+
bool IsShortBreakout()
{
    double currentClose = iClose(_Symbol, PERIOD_CURRENT, 0);
    double currentVolume = iVolume(_Symbol, PERIOD_CURRENT, 0);
    
    // Price below lower band
    bool priceBreakout = currentClose < Lower_Band[1];
    
    // Volume breakout
    bool volumeBreakout = currentVolume > Volume_MA[1] * Volume_Multiplier;
    
    // RSI not oversold
    bool rsiOK = RSI[1] > 30;
    
    // No recent lower low
    bool noRecentLow = true;
    double currentLow = iLow(_Symbol, PERIOD_CURRENT, 0);
    
    for(int i = 1; i <= Lookback_Bars; i++)
    {
        if(iLow(_Symbol, PERIOD_CURRENT, i) < currentLow)
        {
            noRecentLow = false;
            break;
        }
    }
    
    return priceBreakout && volumeBreakout && rsiOK && noRecentLow;
}

//+------------------------------------------------------------------+
//| Open a long position                                              |
//+------------------------------------------------------------------+
void OpenLongPosition()
{
    double ask = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    double stopLoss = Lower_Band[1];
    double bandWidth = Upper_Band[1] - Middle_Band[1];
    double takeProfit = ask + (bandWidth * 2);
    
    double lotSize = CalculateLotSize(MathAbs(ask - stopLoss));
    
    MqlTradeRequest request = {};
    request.action = TRADE_ACTION_DEAL;
    request.symbol = _Symbol;
    request.volume = lotSize;
    request.type = ORDER_TYPE_BUY;
    request.price = ask;
    request.sl = stopLoss;
    request.tp = takeProfit;
    request.deviation = 5;
    request.magic = 234567;
    
    MqlTradeResult result = {};
    OrderSend(request, result);
    
    LastTradeTime = TimeCurrent();
}

//+------------------------------------------------------------------+
//| Open a short position                                             |
//+------------------------------------------------------------------+
void OpenShortPosition()
{
    double bid = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double stopLoss = Upper_Band[1];
    double bandWidth = Middle_Band[1] - Lower_Band[1];
    double takeProfit = bid - (bandWidth * 2);
    
    double lotSize = CalculateLotSize(MathAbs(bid - stopLoss));
    
    MqlTradeRequest request = {};
    request.action = TRADE_ACTION_DEAL;
    request.symbol = _Symbol;
    request.volume = lotSize;
    request.type = ORDER_TYPE_SELL;
    request.price = bid;
    request.sl = stopLoss;
    request.tp = takeProfit;
    request.deviation = 5;
    request.magic = 234567;
    
    MqlTradeResult result = {};
    OrderSend(request, result);
    
    LastTradeTime = TimeCurrent();
}

//+------------------------------------------------------------------+
//| Calculate position size based on risk percentage                   |
//+------------------------------------------------------------------+
double CalculateLotSize(double stopLossPoints)
{
    double accountBalance = AccountInfoDouble(ACCOUNT_BALANCE);
    double riskAmount = accountBalance * (Risk_Percent / 100);
    
    double tickSize = SymbolInfoDouble(_Symbol, SYMBOL_TRADE_TICK_SIZE);
    double tickValue = SymbolInfoDouble(_Symbol, SYMBOL_TRADE_TICK_VALUE);
    double pointValue = tickValue / tickSize;
    
    double lotSize = riskAmount / (stopLossPoints * pointValue);
    double minLot = SymbolInfoDouble(_Symbol, SYMBOL_VOLUME_MIN);
    double maxLot = SymbolInfoDouble(_Symbol, SYMBOL_VOLUME_MAX);
    double lotStep = SymbolInfoDouble(_Symbol, SYMBOL_VOLUME_STEP);
    
    lotSize = MathFloor(lotSize / lotStep) * lotStep;
    lotSize = MathMax(minLot, MathMin(maxLot, lotSize));
    
    return lotSize;
}
