# Smed
## Agent/Adam.

A scripting language for agent-based modelling of economic scenarios

```
include sim

agent Actor {
  let balance = 10000
  let age = 35
}

function Actor.step(state) {
  foreach state.agents {
    ...
  }
}

implement Agent -> sim.current() {
  
}
```
