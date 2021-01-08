# Smed
## Agent/Adam.

A scripting language for agent-based modelling of economic scenarios

```
include countries
include currency

# Notice lexical macros
# ... also mathematical notation
macro convert(a, b, x)
{
  a/b x
}

# Swedish time
gdp: man = denmark.GDP

fun diff-balance(gdp)
{
  open sweden {
    return (gdp + M) - (C + I + X)
  }
}

plot(convert(SEK, DKK, diff-balance(gdp)))

free(gdp)
```
