(() => {
    const numericValue = (text) => {
        const value = Number.parseFloat(text.trim());
        return Number.isFinite(value) ? value : -Infinity;
    };
    window.Tablesort.extend("numeric", (text) => /^[+-]?(?:\d+\.?\d*|\.\d+)(?:e[+-]?\d+)?%?$/i.test(text), (left, right) => {
        const a = numericValue(left);
        const b = numericValue(right);
        return a === b ? 0 : a < b ? 1 : -1;
    });

    const tables = new Set(
        Array.from(document.querySelectorAll("table.sortable, th.sortable"), (element) => element.closest("table"))
    );
    for (const table of tables) {
        if (!table.tHead || !Array.from(table.tBodies).some((body) => body.rows.length > 1)) {
            continue;
        }
        new window.Tablesort(table);
        for (const header of table.tHead.querySelectorAll("th[tabindex='0']")) {
            const button = document.createElement("button");
            button.type = "button";
            button.className = "table-sort-button";
            button.append(...header.childNodes);
            const icon = document.createElement("span");
            icon.className = "table-sort-icon";
            icon.setAttribute("aria-hidden", "true");
            button.append(icon);
            header.append(button);
            header.removeAttribute("tabindex");

            // Let the native button handle activation instead of the header's Enter handler.
            button.addEventListener("keydown", (event) => {
                if (event.key === "Enter") {
                    event.stopPropagation();
                }
            });
        }
    }
})();
