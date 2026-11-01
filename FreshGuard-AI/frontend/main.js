/* FreshGuard AI - Shared frontend logic */

const API = "http://127.0.0.1:5000";

function toast(message, type = "info") {
  const el = document.getElementById("toast");
  if (!el) return;
  el.textContent = message;
  el.className = "toast " + (type === "error" ? "toast error" : "toast");
  el.classList.add("show");
  setTimeout(() => el.classList.remove("show"), 3500);
}

async function api(path, options = {}) {
  const headers = { ...options.headers };
  try {
    const res = await fetch(API + path, {
      credentials: "include",
      ...options,
      headers,
    });
    let data = null;
    try { data = await res.json(); } catch (e) { data = { message: res.statusText }; }
    return { ok: res.ok, status: res.status, data };
  } catch (e) {
    return { ok: false, status: 0, data: { error: "Unable to reach the backend. Start the Flask server (python backend/app.py) and refresh." } };
  }
}

function setAuthUser(user) {
  sessionStorage.setItem("user", JSON.stringify(user));
  document.dispatchEvent(new CustomEvent("authchange", { detail: user }));
}

function getAuthUser() {
  try { return JSON.parse(sessionStorage.getItem("user") || "null"); } catch { return null; }
}

function clearAuth() {
  sessionStorage.removeItem("user");
  document.dispatchEvent(new CustomEvent("authchange", { detail: null }));
}

function isAuthPage() {
  return /login\.html|signup\.html/.test(location.pathname);
}

function requireAuth(redirectTo = "login.html") {
  if (!getAuthUser()) {
    location.href = redirectTo;
  }
}

function requireGuest(redirectTo = "dashboard.html") {
  if (getAuthUser()) {
    location.href = redirectTo;
  }
}

function formatDate(iso) {
  if (!iso) return "-";
  return new Date(iso).toLocaleString();
}

function qualityBadgeClass(status) {
  const s = (status || "").toLowerCase();
  if (s === "good") return "good";
  if (s === "poor") return "poor";
  return "needs";
}

function setupNav() {
  const user = getAuthUser();
  const guestLinks = document.querySelectorAll(".guest-only");
  const authLinks = document.querySelectorAll(".auth-only");
  const userNameEls = document.querySelectorAll(".user-name");
  guestLinks.forEach(el => (el.style.display = user ? "none" : "block"));
  authLinks.forEach(el => (el.style.display = user ? "block" : "none"));
  userNameEls.forEach(el => (el.textContent = user ? user.full_name : ""));
}

document.addEventListener("DOMContentLoaded", () => {
  setupNav();
  const logoutBtn = document.getElementById("logout-btn");
  if (logoutBtn) {
    logoutBtn.addEventListener("click", async (e) => {
      e.preventDefault();
      await api("/api/auth/logout", { method: "POST" });
      clearAuth();
      location.href = "index.html";
    });
  }
});
