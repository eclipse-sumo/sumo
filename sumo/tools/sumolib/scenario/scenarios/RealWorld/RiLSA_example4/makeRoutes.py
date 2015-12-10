
flows = [
   [ "nm", [
       [ "ms", 160, 8 ],  
       [ "me", 60, 10 ],  
       [ "mw", 67, 11 ]  
   ] ],

   [ "wm", [
       [ "me", 662, 11 ],  
       [ "mn", 64, 9 ],  
       [ "ms", 193, 5 ]  
   ] ],

   [ "em", [
       [ "mw", 617, 11 ],  
       [ "mn", 55, 6 ],  
       [ "ms", 50, 4 ]  
   ] ],

   [ "sm", [
       [ "mn", 156, 9 ],  
       [ "me", 59, 3 ],  
       [ "mw", 191, 5 ]  
   ] ]
 
]


fd = open("input_flows.flows.xml", "w")
print >> fd, "<routes>"


for s in flows:
    for d in s[1]:
        id = s[0]+'2'+d[0]
        noLKW = int(float(d[1]) * float(d[2]) / 100.) # hmph, so korrekt?
        noPKW = d[1] - noLKW
        print >> fd, '     <flow id="%sPKW" from="%s" to="%s" number="%s" type="PKW" begin="0" end="3600" departPos="base" arrivalPos="-1" departSpeed="max" departLane="best"/>' % (id, s[0], d[0], noPKW)
        print >> fd, '     <flow id="%sLKW" from="%s" to="%s" number="%s" type="LKW" begin="0" end="3600" departPos="base" arrivalPos="-1" departSpeed="max" departLane="best"/>' % (id, s[0], d[0], noLKW)
    print >> fd, ""

print >> fd, "</routes>\n"
fd.close()

