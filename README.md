# Smed
## Agent/Adam.

A scripting language for mathematical modelling of macroeconomic scenarios.

```kt
class Person: sim.Agent {
  fn new() {
    super()

    self.income = 3 #idk
  }
}

repeat {
  if not sim.run() {
    break
  }
}

plot(sim.history)

print sim.balance
```
