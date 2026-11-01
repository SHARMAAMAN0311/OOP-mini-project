/* FreshGuard AI - Dashboard */

document.addEventListener("DOMContentLoaded", loadDashboard);

async function loadDashboard(){
  const res = await api("/api/dashboard");
  if(!res.ok){
    if(res.status===401){ location.href="login.html"; return; }
    toast("Failed to load dashboard","error"); return;
  }
  const d = res.data;
  setText("stat-total", d.total);
  setText("stat-good", d.good);
  setText("stat-poor", d.poor);
  setText("stat-needs", d.needs_inspection);

  const tbody = document.getElementById("recent-rows");
  const empty = document.getElementById("recent-empty");
  tbody.innerHTML = "";
  const recent = d.recent_inspections || [];
  if(recent.length===0){ tbody.style.display="none"; empty.style.display="block"; return; }
  tbody.style.display=""; empty.style.display="none";
  recent.forEach(r=>{
    const tr = document.createElement("tr");
    const cls = qualityBadgeClass(r.quality_status);
    tr.innerHTML = `
      <td>${r.product_name}</td>
      <td>${r.category}</td>
      <td><span class="badge ${cls}">${r.quality_status}</span></td>
      <td>${formatDate(r.created_at)}</td>
    `;
    tbody.appendChild(tr);
  });
}

function setText(id,val){ const el=document.getElementById(id); if(el) el.textContent = val==null?"0":val; }
