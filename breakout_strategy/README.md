# Breakout Trading Strategy

## Overview

This breakout strategy is designed to capitalize on price movements that break through significant support and resistance levels. It uses Bollinger Bands to identify potential breakout levels and confirm price volatility. The strategy is particularly effective during periods of range consolidation followed by strong directional moves.

## Key Components

1. **Bollinger Bands**:
   - Period: 20
   - Standard Deviation: 2
   - Used to identify potential breakout levels and volatility
   
2. **Support and Resistance**:
   - Calculated using recent swing highs and lows
   - Confirmed by price action (multiple touches)
   - Dynamic levels updated each bar

3. **Volume**:
   - Used to confirm breakout strength
   - Volume should increase during breakout

## Entry Signals

### Long (Buy) Entry:
- Price closes above the upper Bollinger Band
- Volume is above 150% of the 20-period average
- No recent higher high in the last 10 bars
- RSI below 70 (avoid overbought conditions)

### Short (Sell) Entry:
- Price closes below the lower Bollinger Band
- Volume is above 150% of the 20-period average
- No recent lower low in the last 10 bars
- RSI above 30 (avoid oversold conditions)

## Exit Signals

1. **Take Profit**:
   - Primary target: 2 times the Bollinger Band width
   - Secondary target: Previous significant support/resistance level

2. **Stop Loss**:
   - Placed at the opposite Bollinger Band
   - Maximum 2% of account equity
   
3. **Additional Exit Conditions**:
   - Price returns inside Bollinger Bands
   - Volume drops below average
   - Opposite signal appears

## Risk Management

- Maximum risk per trade: 1.5% of account
- Maximum open positions: 2
- No trading during major economic news
- Minimum reward:risk ratio of 1.5:1

## Implementation Notes

The provided MQL5 code includes:
- Dynamic calculation of support/resistance levels
- Volume analysis functions
- Position sizing based on account risk
- Multiple time frame analysis
- Comprehensive trade management

See [BreakoutStrategy.mq5](./BreakoutStrategy.mq5) for the complete implementation.
