function getSearchTerm() {
    const sPageURL = window.location.search.substring(1);
    const sURLVariables = sPageURL.split('&');

    for (const variable of sURLVariables) {
        const sParameterName = variable.split('=');
        if (sParameterName[0] === 'q') {
            return sParameterName[1];
        }
    }
}

$(document).ready(function() {

    if (/msie|trident/i.test(navigator.userAgent)) {
        $('#dark-mode-div').hide();
    }

    let search_term = getSearchTerm(),
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
        const key = e.key;
        let page;
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
            globalThis.location.href = page;
        }
    });

    $('table').addClass('table table-striped table-hover');

    // Improve the scrollspy behavior when users click on a TOC item.
    $(".bs-sidenav a").on("click", function(event) {
        const clicked = event.currentTarget;

        setTimeout(() => {
            let active = $('.nav li.active a');
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

const keyCodes = {
  Backspace: 'backspace',
  Tab: 'tab',
  Enter: 'enter',
  Shift: 'shift',
  Control: 'ctrl',
  Alt: 'alt',
  Pause: 'pause/break',
  CapsLock: 'caps lock',
  Escape: 'escape',
  ' ': 'spacebar',
  PageUp: 'page up',
  PageDown: 'page down',
  End: 'end',
  Home: 'home',
  ArrowLeft: '&larr;',
  ArrowUp: '&uarr;',
  ArrowRight: '&rarr;',
  ArrowDown: '&darr;',
  Insert: 'insert',
  Delete: 'delete',
  0: '0',
  1: '1',
  2: '2',
  3: '3',
  4: '4',
  5: '5',
  6: '6',
  7: '7',
  8: '8',
  9: '9',
  a: 'a',
  b: 'b',
  c: 'c',
  d: 'd',
  e: 'e',
  f: 'f',
  g: 'g',
  h: 'h',
  i: 'i',
  j: 'j',
  k: 'k',
  l: 'l',
  m: 'm',
  n: 'n',
  o: 'o',
  p: 'p',
  q: 'q',
  r: 'r',
  s: 's',
  t: 't',
  u: 'u',
  v: 'v',
  w: 'w',
  x: 'x',
  y: 'y',
  z: 'z',
  Meta: 'Windows / ⌘',
  '*': 'multiply',
  '+': 'add',
  '-': 'subtract',
  '.': '&period;',
  '/': '&quest;',
  F1: 'f1',
  F2: 'f2',
  F3: 'f3',
  F4: 'f4',
  F5: 'f5',
  F6: 'f6',
  F7: 'f7',
  F8: 'f8',
  F9: 'f9',
  F10: 'f10',
  F11: 'f11',
  F12: 'f12',
  F13: 'f13',
  F14: 'f14',
  F15: 'f15',
  F16: 'f16',
  F17: 'f17',
  F18: 'f18',
  F19: 'f19',
  F20: 'f20',
  F21: 'f21',
  F22: 'f22',
  F23: 'f23',
  F24: 'f24',
  NumLock: 'num lock',
  ScrollLock: 'scroll lock',
  ';': '&semi;',
  '=': '&equals;',
  ',': '&comma;',
  '[': '&lsqb;',
  '\\': '&bsol;',
  ']': '&rsqb;',
  "'": '&apos;',
  '`': '&grave;',
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
    video.dataset.youtube = id;

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
    let id = link.dataset.youtube;

	// Create the player
	let player = document.createElement('div');
	player.innerHTML = `<iframe width="560" height="315" src="https://www.youtube-nocookie.com/embed/${id}?autoplay=1" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>`;

	// Inject the player into the UI
	link.replaceWith(player);

}

// Detect clicks on the video thumbnails
document.addEventListener('click', clickHandler);
