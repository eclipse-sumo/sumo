**Startpositionen:**

Urspr. Reihenfolge: `t_0`, `t_1`, `t_2`

BL-redundante Constraints: True

```.text
t_0 ----
        \
t_1 -----------------
                    \
t_2------------------------------------
```

**Aktion:**

Es wird ein swapConstraint für `t_2` und `t_0` aufgerufen.

**Aktuelles Verhalten:**

Aktuell entsteht ein Deadlock, da `t_2` auf `t_1` und `t_0` auf `t_2` wartet - wobei sich `t_1` hinter `t_0` befindet.  

**Gewünschtes Verhalten:**

Es müsste ebenfalls das Constraint `t_2` wartet auf `t_1` getauscht werden. 

**Gewünschte Reihenfolge:**

`t_2`, `t_0`, `t_1`









**Aktuelles Verhalten:**

Aktuell überholen sowohl `t_1` als auch `t_2` das Fahrzeug `t_0`, obwohl `t_0` vor `t_2` weiterhin valide ist.

**Gewünschtes Verhalten:**

Das Constraint `t_2` wartet auf `t_1` gilt weiterhin.

**Gewünschte Reihenfolge:**

`t_2`, `t_0`, `t_1`