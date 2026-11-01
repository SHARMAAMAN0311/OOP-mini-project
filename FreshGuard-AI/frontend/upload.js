/* FreshGuard AI - Upload page logic */

let selectedFile = null;

function showUploadErr(msg) {
  const err = document.getElementById("upload-error");
  if (!err) return;
  err.textContent = msg;
  err.style.display = msg ? "block" : "none";
}

function setLoadingBtn(btn, loading) {
  if (!btn) return;
  btn.disabled = loading;
  btn.innerHTML = loading
    ? '<span class="loader" style="width:20px;height:20px;border-width:3px"></span> Analyzing&#8230;'
    : btn.dataset.label || "Analyze Image";
}

function handleFile(f) {
  if (!f) return;
  if (!f.type.startsWith("image/")) {
    toast("Please select an image file", "error");
    return;
  }
  if (f.size > 8 * 1024 * 1024) {
    toast("File too large (max 8 MB)", "error");
    return;
  }
  selectedFile = f;
  const previewImg = document.getElementById("previewImg");
  const preview = document.getElementById("preview");
  if (previewImg) previewImg.src = URL.createObjectURL(f);
  if (preview) preview.style.display = "block";
}

function removeImage() {
  selectedFile = null;
  const preview = document.getElementById("preview");
  const fileInput = document.getElementById("fileInput");
  if (preview) preview.style.display = "none";
  if (fileInput) fileInput.value = "";
}

document.addEventListener("DOMContentLoaded", () => {
  const dropZone = document.getElementById("dropZone");
  const fileInput = document.getElementById("fileInput");

  // Prevent browser from opening dropped files anywhere on the page
  window.addEventListener("dragover", (e) => e.preventDefault());
  window.addEventListener("drop", (e) => e.preventDefault());

  if (dropZone) {
    dropZone.addEventListener("click", () => {
      if (fileInput) fileInput.click();
    });
    ["dragenter", "dragover"].forEach((evt) =>
      dropZone.addEventListener(evt, (e) => {
        e.preventDefault();
        dropZone.classList.add("drag-over");
      })
    );
    ["dragleave", "drop"].forEach((evt) =>
      dropZone.addEventListener(evt, (e) => {
        e.preventDefault();
        dropZone.classList.remove("drag-over");
      })
    );
    dropZone.addEventListener("drop", (e) => {
      const f = e.dataTransfer && e.dataTransfer.files ? e.dataTransfer.files[0] : null;
      if (f) handleFile(f);
    });
  }

  if (fileInput) {
    fileInput.addEventListener("change", (e) => {
      if (e.target.files[0]) handleFile(e.target.files[0]);
    });
  }

  const btnRemove = document.getElementById("btnRemove");
  const btnReplace = document.getElementById("btnReplace");
  if (btnRemove) btnRemove.addEventListener("click", removeImage);
  if (btnReplace && fileInput)
    btnReplace.addEventListener("click", () => fileInput.click());

  const categoryOptions = document.querySelectorAll(".category-option");
  categoryOptions.forEach((opt) => {
    opt.addEventListener("click", () => {
      categoryOptions.forEach((o) => o.classList.remove("selected"));
      opt.classList.add("selected");
      const radio = opt.querySelector('input[type="radio"]');
      if (radio) radio.checked = true;
    });
  });

  const form = document.getElementById("uploadForm");
  if (form) {
    form.addEventListener("submit", async (e) => {
      e.preventDefault();
      const btn = document.getElementById("analyzeBtn");
      showUploadErr("");
      if (!selectedFile) {
        showUploadErr("Please select an image");
        return;
      }

      const fd = new FormData();
      const productName = (form.product_name && form.product_name.value.trim()) || "";
      const category = (form.category && form.category.value) || "";
      if (!productName) {
        showUploadErr("Product name is required");
        return;
      }
      if (!category) {
        showUploadErr("Please select a category");
        return;
      }
      fd.append("image", selectedFile);
      fd.append("product_name", productName);
      fd.append("category", category);
      fd.append("batch_number", (form.batch_number && form.batch_number.value.trim()) || "");
      fd.append(
        "inspection_location",
        (form.inspection_location && form.inspection_location.value.trim()) || ""
      );

      setLoadingBtn(btn, true);
      const res = await api("/api/inspect", { method: "POST", body: fd });
      setLoadingBtn(btn, false);

      if (res.ok) {
        location.href =
          "result.html?id=" +
          res.data.inspection_id +
          "&mode=" +
          encodeURIComponent(res.data.analysis_mode || "");
      } else {
        showUploadErr(res.data.error || res.statusText || "Inspection failed");
      }
    });
  }
});
