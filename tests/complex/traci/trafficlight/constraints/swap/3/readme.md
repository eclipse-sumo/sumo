**Startpositionen:**

Urspr. Reihenfolge: `t_0`, `t_1`, `t_2`

```.text
t_0 ----
        \
t_1 -----------------
                    \
t_2------------------------------------
```

**Aktion:**

Es wird die Reihenfolge der oberen Fahrzeuge `t_0`, `t_1` getauscht. 

**Aktuelles Verhalten:**

Aktuell überholen sowohl `t_1` als auch `t_2` das Fahrzeug `t_0`, obwohl `t_0` vor `t_2` weiterhin valide ist.

**Gewünschtes Verhalten:**

Das Constraint `t_2` wartet auf `t_1` gilt weiterhin.

**Gewünschte Reihenfolge:**

`t_1`, `t_0`, `t_2`
