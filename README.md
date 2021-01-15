# Smed
## Agent/Adam.

A scripting language for agent-based modelling of economic scenarios

```
include sim

class Actor {
  let balance = 10000
  let age = 35
}

fn Actor.step(state) {
  foreach state.agents {
    ...
  }
}

implement Agent -> sim.current() {
  
}
```
x
