jQuery PaRSS Plugin

1. About

PaRSS is a jQuery script that parses an RSS feed using 
the Google Feed API, and appends a specified number of 
items from the feed to a list element on the page.

2. Usage

To use the PaRSS plugin, create a <ul> or <ol> element 
on your page and give it an ID.

Then, include the jQuery and PaRSS scripts in your 
page, and put a script in the header that calls the 
PaRSS plugin on that element.

  Step 1: Include the jQuery and PaRSS scripts

    Put the following text in the <head> section of your page:

    Code:

    <script type="text/javascript" 
      src="https://ajax.googleapis.com/ajax/libs/jquery/1.4.3/jquery.min.js"></script>
    <script type="text/javascript" 
      src="jquery.parss.js"></script>    
    
  Step 2: Call the jFeed script

    Also in the <head> section of your page, call the jFeed 
    script on your <ul> or <ol> element:

    Code:

    <script type="text/javascript">
    <!--
      $(document).ready(function(){
        $("#feed").PaRSS(
          "http://www.bradwestness.com/feed",  // rss feed url (required)
          5,                                   // number of items (optional)
          "M jS g:i a",                        // date format (optional)
          true                                 // include descriptions? (optional)
        );
      });
    -->
    </script>
    
    In the code above, the $(document).ready(function(){ line 
    indicates that the script should not execute until the page 
    has finished loading. This is necessary because otherwise the 
    script may try to load the RSS feed before the browser has 
    loaded the list element into which the feed should be placed.

    On the next line, $("#feed") indicates the ID of the list 
    element into which the feed items should be placed. In this 
    case, the page must have an element with an ID of "feed," 
    or the script will not execute.

    Code:

    <ul id="feed"></ul>
    
    The list element need not have any <li> elements to begin with, 
    as they will be added by the jFeed script.

  Step 3. Options

    As you can see in the code above, there are several options 
    which can be passed to the jFeed script. These options must 
    always be given in the following order:

      Feed Url (required):
        The URL of the RSS feed to fetch.

      Items to return (optional):
        The number of items from the feed to insert into your 
        list element. If not specified, the default is 4.

      Date Format (optional):
        The date format to use for the date of the items. 
        If no date format is supplied, the date is not displayed.
        The date format uses the PHP date format syntax
        as defined here: http://php.net/manual/en/function.date.php

      Include Descriptions (optional):
        If set to true, a snippet of the content is displayed.
        
        If set to "content", the full content of each item is included.

        If set to "image", the first image from the item is pulled (if any), and a snippet of content is displayed.

  Step 4. Styling PaRSS Lists
    RSS element content created by PaRSS includes several CSS classes which can be used to style your lists:

      <span class="parss-title"></span>
        The titles of the feed items will be wrapped with this tag.
      <span class="parss-date"></span>
        If dates are included in your list, they will be wrapped in this tag.
      <span class="parss-image"></span>
        If show_descriptions is set to image, the image will be wrapped with this tag.
      <div class="parss-description"></div>
        If the feed item description is displayed, it will be wrapped in this tag.

    Paste the following CSS into the <head> section of your document and fill in the commented lines to style your jFeed lists:
    
    Code:

      <style type="text/css">
      <!--
        span.parss-title {
          /* title style definitions here */
        }
        span.parss-date { 
          /* date style definitions here */
        }
        span.parss-image {
          /* image wrapper style definitions here */
        }
        span.parss-image img {
          /* styles for the actual image element */
        }
        div.parss-description {
          /* description style definitions here */
        }
      -->
      </style>
      
3. License

  Copyright (c) 2011 Brad Westness, http://www.bradwestness.com/

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.