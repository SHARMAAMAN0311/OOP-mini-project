/* FreshGuard AI - Auth pages (login/signup) */

async function handleLogin(e) {
  e.preventDefault();
  const form = e.target;
  const btn = form.querySelector("button[type=submit]");
  const err = document.getElementById("login-error");
  const email = form.email.value.trim();
  const password = form.password.value;

  if (!email || !password) {
    showErr(err, "Email and password are required");
    return;
  }
  setLoading(btn, true);
  err.style.display = "none";

  const res = await api("/api/auth/login", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ email, password }),
  });
  setLoading(btn, false);

  if (res.ok) {
    setAuthUser(res.data.user);
    toast("Welcome back!");
    location.href = "dashboard.html";
  } else {
    showErr(err, res.data.error || "Login failed");
  }
}

function showErr(el, msg) {
  if (!el) return;
  el.textContent = msg;
  el.style.display = "block";
}

function setLoading(btn, loading) {
  if (!btn) return;
  btn.disabled = loading;
  btn.innerHTML = loading ? '<span class="loader" style="width:20px;height:20px;border-width:3px"></span> Working...' : btn.dataset.label || "Sign in";
}

async function handleSignup(e) {
  e.preventDefault();
  const form = e.target;
  const btn = form.querySelector("button[type=submit]");
  const err = document.getElementById("signup-error");
  const data = {
    full_name: form.full_name.value.trim(),
    email: form.email.value.trim().toLowerCase(),
    password: form.password.value,
    confirm_password: form.confirm_password.value,
  };

  if (!data.full_name || !data.email || !data.password || !data.confirm_password) {
    showErr(err, "All fields are required");
    return;
  }
  if (data.password !== data.confirm_password) {
    showErr(err, "Passwords do not match");
    return;
  }
  if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(data.email)) {
    showErr(err, "Invalid email address");
    return;
  }
  if (data.password.length < 6) {
    showErr(err, "Password must be at least 6 characters");
    return;
  }

  setLoading(btn, true);
  err.style.display = "none";

  const res = await api("/api/auth/signup", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  });
  setLoading(btn, false);

  if (res.ok) {
    setAuthUser(res.data.user);
    toast("Account created!");
    location.href = "dashboard.html";
  } else {
    showErr(err, res.data.error || "Signup failed");
  }
}

function togglePassword(id) {
  const input = document.getElementById(id);
  if (!input) return;
  input.type = input.type === "password" ? "text" : "password";
}

function setupPasswordStrength() {
  const input = document.getElementById("signup-password");
  const indicator = document.getElementById("password-strength");
  if (!input || !indicator) return;
  input.addEventListener("input", () => {
    const p = input.value;
    let score = 0;
    if (p.length >= 6) score++;
    if (p.length >= 8) score++;
    if (/[A-Z]/.test(p)) score++;
    if (/[0-9]/.test(p)) score++;
    if (/[^A-Za-z0-9]/.test(p)) score++;
    const labels = ["", "Weak", "Fair", "Good", "Strong", "Very Strong"];
    const colors = ["", "#FF647C", "#FFB547", "#42E695", "#42E695", "#42E695"];
    indicator.textContent = labels[score] || "";
    indicator.style.color = colors[score] || "";
  });
}

// Wire up form listeners once the page and this script are ready.
document.addEventListener("DOMContentLoaded", () => {
  const loginForm = document.getElementById("loginForm");
  if (loginForm) {
    loginForm.addEventListener("submit", handleLogin);
  }
  const signupForm = document.getElementById("signupForm");
  if (signupForm) {
    signupForm.addEventListener("submit", handleSignup);
  }
  setupPasswordStrength();
});
