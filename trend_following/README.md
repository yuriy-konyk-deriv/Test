# Trend Following Strategy

## Overview

This trend following strategy is designed to identify and trade with established market trends using a combination of Moving Averages (MA) and the Average Directional Index (ADX). The strategy aims to enter trades in the direction of the trend when momentum is strong and exit when the trend shows signs of weakening.

## Key Components

1. **Moving Averages**:
   - Fast EMA (8 periods): Responds quickly to price changes
   - Slow EMA (21 periods): Provides trend direction
   - The crossover of these MAs helps identify trend changes

2. **Average Directional Index (ADX)**:
   - Period: 14
   - Used to measure trend strength
   - ADX > 25 indicates a strong trend
   - ADX < 20 indicates a weak or no trend

## Entry Signals

### Long (Buy) Entry:
- Fast EMA crosses above Slow EMA
- ADX is above 25 (strong trend)
- Current price is above both EMAs

### Short (Sell) Entry:
- Fast EMA crosses below Slow EMA
- ADX is above 25 (strong trend)
- Current price is below both EMAs

## Exit Signals

1. **Take Profit**:
   - Set at 2 times the Average True Range (ATR)
   - Adjusts based on market volatility

2. **Stop Loss**:
   - Set at 1 times the ATR below/above entry price
   - Provides dynamic risk management

3. **Additional Exit Conditions**:
   - ADX falls below 20 (trend weakening)
   - Fast EMA crosses opposite direction
   - Trailing stop activated after 1 ATR of profit

## Risk Management

- Position size: Maximum 2% risk per trade
- Risk:Reward ratio: Minimum 1:2
- No trading during major news events
- Maximum 3 concurrent trades

## Implementation Notes

The provided MQL5 code includes:
- Automatic calculation of position size
- Dynamic stop loss and take profit levels
- Real-time monitoring of exit conditions
- Comprehensive error handling
- Customizable parameters through inputs

See [TrendFollowing.mq5](./TrendFollowing.mq5) for the complete implementation.
