function elem(r){
 if(r[0] == "<"){
  var n = document.createElement(r.substring(1, r.length - 1));
  var s = arguments[1] || {};
  var i, j;
  for(i in s){
			if(i == "css") i = "style";
			if(s[i] instanceof Object){
				for(j in s[i])
				 n[i][j] = s[i][j];
			} else
    n[i] = s[i];
  }
  s = arguments[2] || {};
  for(i in s)
   n.addEventListener(i, s[i]);
  return n;
 } else
	 return document.querySelector(r);
}

function on(type, callback){
	if(type == "ready") type = "DOMContentLoaded";
	addEventListener(type, callback);
}

HTMLElement.prototype.on = function(type, callback){
 this.addEventListener(type, callback);
}

HTMLElement.prototype.elem = function(r){
 return this.querySelector(r);
}

HTMLElement.prototype.append = function(n){
 if(typeof n == "string") n = elem.apply(void 0, arguments);
 return this.appendChild(n);
}

function lequal(){
 for(var i = 1, l = arguments.length; i < l; ++i)
  if(arguments[i - 1] > arguments[i]) return false;
 return true;
}
