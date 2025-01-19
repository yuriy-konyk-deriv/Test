//+------------------------------------------------------------------+
//|                                                   TrendFollowing.mq5 |
//|                                  Copyright 2024, Trading Strategies   |
//+------------------------------------------------------------------+
#property copyright "Copyright 2024, Trading Strategies"
#property link      "https://www.yourwebsite.com"
#property version   "1.00"
#property strict

// Input Parameters
input int      FastEMA_Period = 8;        // Fast EMA Period
input int      SlowEMA_Period = 21;       // Slow EMA Period
input int      ADX_Period     = 14;       // ADX Period
input double   ADX_Threshold  = 25;       // ADX Threshold for Trend Strength
input double   RiskPercent    = 2;        // Risk Percent per Trade
input double   ATR_Multiple   = 2;        // ATR Multiple for Take Profit

// Handle Declarations
int FastEMA_Handle;
int SlowEMA_Handle;
int ADX_Handle;
int ATR_Handle;

// Global Variables
double FastEMA[], SlowEMA[], ADX[], ATR[];
int bars_total;

//+------------------------------------------------------------------+
//| Expert initialization function                                     |
//+------------------------------------------------------------------+
int OnInit()
{
    // Initialize indicator handles
    FastEMA_Handle = iMA(_Symbol, PERIOD_CURRENT, FastEMA_Period, 0, MODE_EMA, PRICE_CLOSE);
    SlowEMA_Handle = iMA(_Symbol, PERIOD_CURRENT, SlowEMA_Period, 0, MODE_EMA, PRICE_CLOSE);
    ADX_Handle = iADX(_Symbol, PERIOD_CURRENT, ADX_Period);
    ATR_Handle = iATR(_Symbol, PERIOD_CURRENT, 14);
    
    // Allocate arrays
    ArraySetAsSeries(FastEMA, true);
    ArraySetAsSeries(SlowEMA, true);
    ArraySetAsSeries(ADX, true);
    ArraySetAsSeries(ATR, true);
    
    return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                   |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
    // Clean up
    ArrayFree(FastEMA);
    ArrayFree(SlowEMA);
    ArrayFree(ADX);
    ArrayFree(ATR);
}

//+------------------------------------------------------------------+
//| Expert tick function                                              |
//+------------------------------------------------------------------+
void OnTick()
{
    // Update indicator values
    CopyBuffer(FastEMA_Handle, 0, 0, 3, FastEMA);
    CopyBuffer(SlowEMA_Handle, 0, 0, 3, SlowEMA);
    CopyBuffer(ADX_Handle, 0, 0, 3, ADX);
    CopyBuffer(ATR_Handle, 0, 0, 1, ATR);
    
    // Check for open positions
    if(PositionsTotal() > 0) return;
    
    // Check entry conditions
    if(IsLongEntry())
    {
        OpenLongPosition();
    }
    else if(IsShortEntry())
    {
        OpenShortPosition();
    }
}

//+------------------------------------------------------------------+
//| Check for long entry conditions                                    |
//+------------------------------------------------------------------+
bool IsLongEntry()
{
    // Fast EMA crossed above Slow EMA
    bool crossover = FastEMA[1] > SlowEMA[1] && FastEMA[2] <= SlowEMA[2];
    
    // Strong trend
    bool strongTrend = ADX[1] > ADX_Threshold;
    
    // Price above EMAs
    bool priceAboveEMA = SymbolInfoDouble(_Symbol, SYMBOL_ASK) > FastEMA[1] && 
                         SymbolInfoDouble(_Symbol, SYMBOL_ASK) > SlowEMA[1];
    
    return crossover && strongTrend && priceAboveEMA;
}

//+------------------------------------------------------------------+
//| Check for short entry conditions                                   |
//+------------------------------------------------------------------+
bool IsShortEntry()
{
    // Fast EMA crossed below Slow EMA
    bool crossunder = FastEMA[1] < SlowEMA[1] && FastEMA[2] >= SlowEMA[2];
    
    // Strong trend
    bool strongTrend = ADX[1] > ADX_Threshold;
    
    // Price below EMAs
    bool priceBelowEMA = SymbolInfoDouble(_Symbol, SYMBOL_BID) < FastEMA[1] && 
                         SymbolInfoDouble(_Symbol, SYMBOL_BID) < SlowEMA[1];
    
    return crossunder && strongTrend && priceBelowEMA;
}

//+------------------------------------------------------------------+
//| Open a long position                                              |
//+------------------------------------------------------------------+
void OpenLongPosition()
{
    double ask = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
    double stopLoss = ask - ATR[0];
    double takeProfit = ask + (ATR[0] * ATR_Multiple);
    
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
    request.magic = 123456;
    
    MqlTradeResult result = {};
    OrderSend(request, result);
}

//+------------------------------------------------------------------+
//| Open a short position                                             |
//+------------------------------------------------------------------+
void OpenShortPosition()
{
    double bid = SymbolInfoDouble(_Symbol, SYMBOL_BID);
    double stopLoss = bid + ATR[0];
    double takeProfit = bid - (ATR[0] * ATR_Multiple);
    
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
    request.magic = 123456;
    
    MqlTradeResult result = {};
    OrderSend(request, result);
}

//+------------------------------------------------------------------+
//| Calculate position size based on risk percentage                   |
//+------------------------------------------------------------------+
double CalculateLotSize(double stopLossPoints)
{
    double accountBalance = AccountInfoDouble(ACCOUNT_BALANCE);
    double riskAmount = accountBalance * (RiskPercent / 100);
    
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
