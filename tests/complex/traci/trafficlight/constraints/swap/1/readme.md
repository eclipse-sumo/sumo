**Startpositionen:**

Urspr. Reihenfolge: `t_0`, `t_1`, `t_2`

BL-redundante Constraints: True

```.text
t_0 ----
        |
t_1 -----------------
                    |
t_2------------------------------------
```

**Aktion:**

Es wird ein swapConstraint für `t_2` und `t_1` aufgerufen.

**Aktuelles Verhalten:**

Enstpricht mit BL-redundanten Constraints dem gewünschten Verhalten.

**Gewünschtes Verhalten:**

Der `swapConstraints`-Call führt zu der erwarteten Reihenfolge. 

**Gewünschte Reihenfolge:**

`t_0`, `t_2`,  `t_1`