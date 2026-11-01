/* FreshGuard AI - History */

let allRecords = [];

document.addEventListener("DOMContentLoaded", loadHistory);

async function loadHistory(){
  const res = await api("/api/inspections");
  if(!res.ok){
    if(res.status===401){ location.href="login.html"; return; }
    toast("Failed to load history","error"); return;
  }
  allRecords = res.data || [];
  render(allRecords);
  fillCategories(allRecords);
  document.getElementById("exportBtn").addEventListener("click", exportCSV);
}

function fillCategories(records){
  const sel = document.getElementById("catFilter");
  const cats = [...new Set(records.map(r=>r.category))];
  sel.innerHTML = '<option value="">All Categories</option>' + cats.map(c=>`<option value="${c}">${c}</option>`).join("");
}

function render(records){
  const tbody = document.getElementById("historyRows");
  const empty = document.getElementById("historyEmpty");
  tbody.innerHTML = "";
  if(!records.length){ tbody.style.display="none"; empty.style.display="block"; return; }
  tbody.style.display=""; empty.style.display="none";
  records.forEach(r=>{
    const cls = qualityBadgeClass(r.quality_status);
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>#${r.id}</td>
      <td>${formatDate(r.created_at)}</td>
      <td>${r.product_name}</td>
      <td>${r.category}</td>
      <td><span class="badge ${cls}">${r.quality_status}</span></td>
      <td>${r.analysis_mode}</td>
      <td class="actions-cell">
        <a href="result.html?id=${r.id}&mode=${encodeURIComponent(r.analysis_mode||'')}" class="btn btn-outline" style="padding:6px 12px;font-size:0.85rem">View</a>
        <button class="btn btn-danger" onclick="openDeleteModal(${r.id})" style="padding:6px 12px;font-size:0.85rem">Delete</button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

function applyFilters(){
  const q = document.getElementById("searchInput").value.toLowerCase();
  const cat = document.getElementById("catFilter").value;
  const date = document.getElementById("dateFilter").value;
  const filtered = allRecords.filter(r=>{
    const matchQ = r.product_name.toLowerCase().includes(q) || r.category.toLowerCase().includes(q);
    const matchC = !cat || r.category===cat;
    const matchD = !date || r.created_at.startsWith(date);
    return matchQ && matchC && matchD;
  });
  render(filtered);
}
document.getElementById("searchInput").addEventListener("input", applyFilters);
document.getElementById("catFilter").addEventListener("change", applyFilters);
document.getElementById("dateFilter").addEventListener("change", applyFilters);

async function exportCSV(){
  const res = await fetch(API+"/api/reports/history.csv", { credentials:"include" });
  if(!res.ok){ toast("Export failed","error"); return; }
  const blob = await res.blob();
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url; a.download = "inspection_history.csv"; a.click();
  URL.revokeObjectURL(url);
}
</script>