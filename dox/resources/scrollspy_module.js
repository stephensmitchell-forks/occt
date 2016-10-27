/**
 * Created by vpv on 6/20/2016.
 */

'use strict';
$(document).ready(function () {
        var tableOfContents = $('.toc');
        var tableOfContentsWidth = tableOfContents.outerWidth();
        var textBlock = $('.textblock');
        var contents = $('.contents');
        var startTop = 155;
        var navPath = $('#nav-path');
        var docContent = $('#doc-content');
        var topContents = Math.abs(contents[0].getBoundingClientRect().top);
        var headerHeight = $('.header').outerHeight();
        var topHeight = $('#top').outerHeight();
        var lastId = "";
        var anchorColor = '#4665A2';
        var anchorActiveColor = '#07ADE8';
        var sideNav = $('#side-nav');
        var textBlockWidth = null;
        var menuItems = tableOfContents.find("a");
        var cur = null;
        var activeNode = null; // current menu item;
        // Anchors corresponding to menu items
        var scrollItems = menuItems.map(function () {
            var item = $($(this).attr("href"));
            if (item.length) {
                return item;
            }
        });
        var isLock = false;

        $(document.body).css({
            'overflow-y': 'hidden'
        });


        tableOfContents.css({
            'position': 'relative',
            'right': '10px',
            'top': '10px',
            'margin-left': '20px'
        });


        onResize();
        highlightTable();
        sideNav.resize(onResize);

        $(window).resize(onResize);

        function onResize(event) {
            showHideMenu();
            moveTable();
            contents.find('img').each(function () {
                    if ($(this).outerWidth() > textBlockWidth || $(this).outerWidth() < $(this)[0].naturalWidth) {
                        $(this).css({
                            'width': textBlockWidth
                        });
                    }
                }
            )
        }

        function moveTable(event) {

            var addDelta = 0;
            if ($('.headertitle')[0].getBoundingClientRect().top > 0) {
                addDelta = 0;
            } else {

                addDelta = -headerHeight;
            }



            var currentTop = tableOfContents.offset().top;
            // console.log('currentTop: ' + currentTop);
            var delta = startTop - currentTop + addDelta;
            // console.log('delta: ' + delta);
            tableOfContents.css({'top': parseInt(tableOfContents.css('top')) + delta + 'px'});
            if (activeNode) {
                var activeNodeBounds = activeNode[0].getBoundingClientRect();
                if (activeNodeBounds.top > $(document).outerHeight() / 2.8) {
                    tableOfContents.css({'top': parseInt(tableOfContents.css('top')) - parseInt((activeNodeBounds.top - ($(document).outerHeight() / 2.8)).toFixed()) + 'px'});

                }
            }
        }

        function checkIfLastItemsVisible() {
            var result = scrollItems.map(function() {
                if($(this).parent()[0].getBoundingClientRect().bottom > 0 && ($(this).parent().offset().top - topHeight > $(document).outerHeight())) {
                   return $(this).parent();
                }
            });
            if(result.length > 0) {
              return false;
            } else {
                return true;
            }
        }
        tableOfContents[0].addEventListener('click', highlightTableItem);
        function highlightTableItem(event) {
            var target = event.target.closest('a');
            var id = target ? target.hash : "";
            id = id.replace("#", '');
            if (lastId !== id) {

                // Set/remove active class
                activeNode = menuItems.filter("[href='#" + id + "']");
                var lastActiveNode = menuItems.filter("[href='#" + lastId + "']");
                activeNode.css({'color': anchorActiveColor});
                lastActiveNode.css({'color': anchorColor});
                lastId = id;
                isLock = true;
            }
        }
        function highlightTable() {
            // Get id of current scroll item
             cur = scrollItems.map(function () {
                if (true) {

                    return this;
                }
            });
            var curItem = 0;
            for(var i =0; i < cur.length; i++) {
                if(cur[i].parent().offset().top - topHeight >0) {
                    curItem = cur[i];
                    break;
                }
            }
            if(curItem) {
                cur = curItem;
            } else {
                cur = cur[cur.length - 1];
            }


            var id = cur && cur.length ? cur[0].id : "";

            if (lastId !== id) {

                // Set/remove active class
                activeNode = menuItems.filter("[href='#" + id + "']");
                var lastActiveNode = menuItems.filter("[href='#" + lastId + "']");
                activeNode.css({'color': anchorActiveColor});
                lastActiveNode.css({'color': anchorColor});
                lastId = id;
            }
        }

        function showHideMenu() {
            textBlockWidth = contents.outerWidth() - (tableOfContentsWidth + 40);
            if (textBlockWidth < tableOfContentsWidth * 2 && tableOfContents.is(':visible')) {
                tableOfContents.hide();
                tableOfContents.width(0);
                textBlockWidth = contents.outerWidth() - tableOfContents.outerWidth() - 20;
                textBlock.width(textBlockWidth-20);
            } else if (!tableOfContents.is(':visible') && textBlockWidth > tableOfContentsWidth * 2) {
                tableOfContents.show();
                tableOfContents.width(tableOfContentsWidth -20);
                textBlockWidth = contents.outerWidth() - tableOfContents.outerWidth() - 20;
                textBlock.width(textBlockWidth);
            } else if (tableOfContents.is(':visible') && textBlockWidth > tableOfContentsWidth * 2) {
                textBlockWidth = contents.outerWidth() - tableOfContents.outerWidth() - 20;
                textBlock.width(textBlockWidth -20);
            }
        }

        function getCurrentMenuItem() {
            return menuItems.filter(function(index, item) {
                return item.getAttribute('href') == '#'+lastId
            })
        }

        docContent.scroll(function (event) {
            moveTable(event);
            if(!isLock) {
                highlightTable();
            }
            isLock = false;

        });


    }
);