function elem(selector, attributes, events){
    if(selector[0] == "<"){
        selector = selector.substring(1, selector.length - 1);
        let node = document.createElement(selector);

        let i, j;
        if(attributes){
            for(i in attributes){
                if(attributes[i] instanceof Object){
                    for(j in attributes[i])
                        node[i][j] = attributes[i][j];
                } else
                    node[i] = attributes[i];
            }
        }

        if(events){
            for(i in events)
                node.addEventListener(i, events[i]);
        }

        return node;
    } else
        return document.querySelector(selector);
}

function elems(selector){
    return document.querySelectorAll(selector);
}

function on(type, callback){
    if(type == "ready")
        type = "DOMContentLoaded";

    addEventListener(type, callback);
}

HTMLElement.prototype.on = function(type, callback){
    this.addEventListener(type, callback);
};

HTMLElement.prototype.elem = function(r){
    return this.querySelector(r);
};

HTMLElement.prototype.append = function(node){
    if(typeof node == "string")
        node = elem.apply(null, arguments);

    return this.appendChild(node);
};

NodeList.prototype.forEach = Array.prototype.forEach;

function lequal(){
    for(let i = 1, l = arguments.length; i < l; ++i){
        if(arguments[i - 1] > arguments[i])
            return false;
    }

    return true;
}

// languages offered in the language selection - this should match the options (and order) from sumo-gui / netedit (GUIMainWindow::buildLanguageMenu)
const LANGUAGES = {
    "en": "English",
//    "de": "Deutsch",
    "es": "Español",
//    "pt": "Português",
//    "fr": "Français",
//    "it": "Italiano",
//    "zh": "简体中文",
//    "zh-Hant": "繁體中文",
//    "tr": "Türkçe",
//    "ja": "日本語",
//    "ko": "한국어"
};

let translations = {};
let currentLanguage = "en";
const originalTexts = new WeakMap();

function TL(msgid){
    return translations[msgid] || msgid;
}

// marks a string for translation without translating it (for elements translated later by translatePage)
function N_(msgid){
    return msgid;
}

// replaces each "%" in the translated string with the next argument (like the TLF macro in SUMO)
function TLF(msgid){
    let msg = TL(msgid);
    for(let i = 1; i < arguments.length && msg.indexOf("%") >= 0; ++i)
        msg = msg.replace("%", () => String(arguments[i]));
    return msg;
}

// returns the code from LANGUAGES matching the given language (like "de-DE") or null
function matchLanguage(lang){
    let parts = String(lang).split(";")[0].trim().replace(/_/g, "-").toLowerCase().split("-");
    if(parts[0] === "zh" && ["tw", "hk", "mo"].includes(parts[1]))
        parts = ["zh", "hant"];
    for(let i = parts.length; i > 0; --i){
        const code = Object.keys(LANGUAGES).find(c => c.toLowerCase() === parts.slice(0, i).join("-"));
        if(code)
            return code;
    }
    return null;
}

// the language from the url parameter "lang" or else the preferred language of the browser
function getLanguage(){
    const lang = new URLSearchParams(globalThis.location.search).get("lang");
    const candidates = (lang ? [lang] : []).concat(navigator.languages || [navigator.language]);
    for(const candidate of candidates){
        const code = matchLanguage(candidate || "");
        if(code)
            return code;
    }
    return "en";
}

async function loadTranslations(lang){
    translations = {};
    currentLanguage = lang;
    document.documentElement.lang = lang;
    // translations are delivered by the web server of osmWebWizard.py, there is no server in file mode
    if(lang === "en" || globalThis.location.protocol === "file:")
        return;
    try {
        const response = await fetch("translation?lang=" + encodeURIComponent(lang));
        if(response.ok)
            translations = (await response.json()).messages || {};
    } catch(e){
        translations = {};
    }
}

// translates all elements marked with "data-i18n" (text content) or "data-i18n-attrs" (attribute list),
// the original strings are kept, so this can be called again after loading another language
function translatePage(root){
    root.querySelectorAll("[data-i18n]").forEach(function(node){
        node.childNodes.forEach(function(child){
            if(child.nodeType !== Node.TEXT_NODE)
                return;
            if(!originalTexts.has(child))
                originalTexts.set(child, child.nodeValue);
            const original = originalTexts.get(child);
            const msgid = original.trim().replace(/\s+/g, " ");
            if(msgid){
                const match = original.match(/^(\s*)[\s\S]*?(\s*)$/);
                child.nodeValue = match[1] + TL(msgid) + match[2];
            }
        });
    });
    root.querySelectorAll("[data-i18n-attrs]").forEach(function(node){
        if(!originalTexts.has(node))
            originalTexts.set(node, {});
        const originals = originalTexts.get(node);
        node.getAttribute("data-i18n-attrs").split(/\s+/).forEach(function(attr){
            if(!attr || !node.hasAttribute(attr))
                return;
            if(!(attr in originals))
                originals[attr] = node.getAttribute(attr);
            node.setAttribute(attr, TL(originals[attr].trim().replace(/\s+/g, " ")));
        });
    });
}
