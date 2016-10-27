

x=[]
for i in range(1000):
   x.append(i*100) 

for time in range(60):
    print 'Now time',time
    for i in range(1000):
        x[i] = x[i]+ 5.0
        print '    Vehicle number',i,'at time',time,'s is at position',x[i],'m'
        
        


