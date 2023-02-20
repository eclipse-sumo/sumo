function getSearchTerm(){
    var sPageURL = window.location.search.substring(1);
    var sURLVariables = sPageURL.split('&');
    for (var i = 0; i < sURLVariables.length; i++)
    {
        var sParameterName = sURLVariables[i].split('=');
        if (sParameterName[0] == 'q')
        {
            return sParameterName[1];
        }
    }
}

$(document).ready(function() {

    if (navigator.userAgent.match(/msie|trident/i)) {
        $('#dark-mode-div').hide();
      }

    var search_term = getSearchTerm(),
        $search_modal = $('#mkdocs_search_modal'),
        $keyboard_modal = $('#mkdocs_keyboard_modal');

    if(search_term){
        $search_modal.modal();
    }

    // make sure search input gets autofocus everytime modal opens.
    $search_modal.on('shown.bs.modal', function () {
        $search_modal.find('#gsc-i-id1').focus();
    });

    // Close search modal when result is selected
    // The links get added later so listen to parent
    $('#mkdocs-search-results').click(function(e) {
      if ($(e.target).is('a')) {
        $search_modal.modal('hide');
      }
    });

    // Populate keyboard modal with proper Keys
    $keyboard_modal.find('.help.shortcut kbd')[0].innerHTML = keyCodes[shortcuts.help];
    $keyboard_modal.find('.search.shortcut kbd')[0].innerHTML = keyCodes[shortcuts.search];
    $keyboard_modal.find('.edit.shortcut kbd')[0].innerHTML = keyCodes[shortcuts.edit];

    // Keyboard navigation
    document.addEventListener("keydown", function(e) {
        if ($(e.target).is(':input')) return true;
        var key = e.which || e.keyCode || window.event && window.event.keyCode;
        var page;
        switch (key) {
            case shortcuts.search:
                e.preventDefault();
                $keyboard_modal.modal('hide');
                $search_modal.modal('show');
                $search_modal.find('#gsc-i-id1').focus();
                break;
            case shortcuts.edit:
                page = $('.navbar a[rel="edit"]:first').prop('href');
                break;
            case shortcuts.help:
                $search_modal.modal('hide');
                $keyboard_modal.modal('show');
                break;
            default: break;
        }
        if (page) {
            $keyboard_modal.modal('hide');
            window.location.href = page;
        }
    });

    $('table').addClass('table table-striped table-hover');

    // Improve the scrollspy behaviour when users click on a TOC item.
    $(".bs-sidenav a").on("click", function() {
        var clicked = this;
        setTimeout(function() {
            var active = $('.nav li.active a');
            active = active[active.length - 1];
            if (clicked !== active) {
                $(active).parent().removeClass("active");
                $(clicked).parent().addClass("active");
            }
        }, 50);
    });

});

$('body').scrollspy({
    target: '.bs-sidebar',
    offset: 100
});

/* Prevent disabled links from causing a page reload */
$("li.disabled a").click(function() {
    event.preventDefault();
});

var keyCodes = {
  8: 'backspace',
  9: 'tab',
  13: 'enter',
  16: 'shift',
  17: 'ctrl',
  18: 'alt',
  19: 'pause/break',
  20: 'caps lock',
  27: 'escape',
  32: 'spacebar',
  33: 'page up',
  34: 'page down',
  35: 'end',
  36: 'home',
  37: '&larr;',
  38: '&uarr;',
  39: '&rarr;',
  40: '&darr;',
  45: 'insert',
  46: 'delete',
  48: '0',
  49: '1',
  50: '2',
  51: '3',
  52: '4',
  53: '5',
  54: '6',
  55: '7',
  56: '8',
  57: '9',
  65: 'a',
  66: 'b',
  67: 'c',
  68: 'd',
  69: 'e',
  70: 'f',
  71: 'g',
  72: 'h',
  73: 'i',
  74: 'j',
  75: 'k',
  76: 'l',
  77: 'm',
  78: 'n',
  79: 'o',
  80: 'p',
  81: 'q',
  82: 'r',
  83: 's',
  84: 't',
  85: 'u',
  86: 'v',
  87: 'w',
  88: 'x',
  89: 'y',
  90: 'z',
  91: 'Left Windows Key / Left ⌘',
  92: 'Right Windows Key',
  93: 'Windows Menu / Right ⌘',
  96: 'numpad 0',
  97: 'numpad 1',
  98: 'numpad 2',
  99: 'numpad 3',
  100: 'numpad 4',
  101: 'numpad 5',
  102: 'numpad 6',
  103: 'numpad 7',
  104: 'numpad 8',
  105: 'numpad 9',
  106: 'multiply',
  107: 'add',
  109: 'subtract',
  110: 'decimal point',
  111: 'divide',
  112: 'f1',
  113: 'f2',
  114: 'f3',
  115: 'f4',
  116: 'f5',
  117: 'f6',
  118: 'f7',
  119: 'f8',
  120: 'f9',
  121: 'f10',
  122: 'f11',
  123: 'f12',
  124: 'f13',
  125: 'f14',
  126: 'f15',
  127: 'f16',
  128: 'f17',
  129: 'f18',
  130: 'f19',
  131: 'f20',
  132: 'f21',
  133: 'f22',
  134: 'f23',
  135: 'f24',
  144: 'num lock',
  145: 'scroll lock',
  186: '&semi;',
  187: '&equals;',
  188: '&comma;',
  189: '&hyphen;',
  190: '&period;',
  191: '&quest;',
  192: '&grave;',
  219: '&lsqb;',
  220: '&bsol;',
  221: '&rsqb;',
  222: '&apos;',
};

////////////////////////////////////////////////////
// Copy code - based on https://www.roboleary.net/2022/01/13/copy-code-to-clipboard-blog.html
////////////////////////////////////////////////////

const copyButtonLabel = "Copy";

// you can use a class selector instead if you, or the syntax highlighting library adds one to the 'pre'. 
let blocks = document.querySelectorAll("pre");

blocks.forEach((block) => {
  // only add button if browser supports Clipboard API
  if (navigator.clipboard) {
    let button = document.createElement("button");
    button.classList.add("copyCodeButton");
    button.innerText = copyButtonLabel;
    button.addEventListener("click", copyCode);
    block.appendChild(button);
  }
});

async function copyCode(event) {
  const button = event.srcElement;
  const pre = button.parentElement;
  let code = pre.querySelector("code");
  let text = code.innerText;
  await navigator.clipboard.writeText(text);
  
  button.innerText = "Copied!";
  button.style.color = "#338033";
  
  setTimeout(()=> {
    button.innerText = copyButtonLabel;
    button.style.color = "#1e1e1e";
  },1000)
}

////////////////////////////////////////////////////
// https://gomakethings.com/how-to-lazy-load-youtube-videos-with-vanilla-javascript/
////////////////////////////////////////////////////

// Get all of the videos
let videos = document.querySelectorAll('[data-youtube]');

for (let video of videos) {
	// Get the video ID
	let id = new URL(video.href).searchParams.get('v');
	
    // Add the ID to the data-youtube attribute
	video.setAttribute('data-youtube', id);

	// Add a role of button
	video.setAttribute('role', 'button');

	// Add a thumbnail
	video.innerHTML =
		`${video.textContent}<br><div class="video-container">
        <img alt="" class="tutorial-video" src="https://img.youtube.com/vi/${id}/maxresdefault.jpg">
        <div class="centered video-text">Click to load external resources from YouTube.</div>
        </div>`;
}

/**
 * Handle click events on the video thumbnails
 * @param  {Event} event The event object
 */
function clickHandler (event) {

	// Get the video link
	let link = event.target.closest('[data-youtube]');
	if (!link) return;

	// Prevent the URL from redirecting users
	event.preventDefault();

	// Get the video ID
	let id = link.getAttribute('data-youtube');

	// Create the player
	let player = document.createElement('div');
	player.innerHTML = `<iframe width="560" height="315" src="https://www.youtube-nocookie.com/embed/${id}?autoplay=1" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>`;

	// Inject the player into the UI
	link.replaceWith(player);

}

// Detect clicks on the video thumbnails
document.addEventListener('click', clickHandler);