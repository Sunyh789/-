// app.js - 修正版（完整文件）
// 功能：用户注册/登录（教师/学生）、教师添加/删除/修改/查询学生、学生查看个人信息
// 持久化：使用 localStorage，keys 为 "users" 和 "students"
// 注意：请确保此文件保存为 UTF-8（无 BOM）

// 初始数据（仅在 localStorage 无对应数据时使用）
let users = JSON.parse(localStorage.getItem('users')) || [
    { user_id: 1001, password: '123456', identity: '教师' },
    { user_id: 2001, password: '123456', identity: '学生' }
];

let students = JSON.parse(localStorage.getItem('students')) || [
    { id: 2023001, name: '张三', age: 20, grade: '大一', profession: '计算机', Math: 85, English: 78, Programming: 92 },
    { id: 2023002, name: '李四', age: 19, grade: '大一', profession: '软件工程', Math: 92, English: 88, Programming: 95 },
    { id: 2023003, name: '王五', age: 21, grade: '大二', profession: '物联网', Math: 76, English: 82, Programming: 88 }
];

let currentUser = null;
const MAX_USERS = 1000;

// 页面加载：先从 localStorage 载入数据，再渲染表格
document.addEventListener('DOMContentLoaded', function () {
    loadFromLocalStorage();
    renderStudentsTable();
});

// 切换登录/注册标签（更稳健，兼容不同 DOM 排序）
function switchTab(tab) {
    document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
    document.querySelectorAll('.form-content').forEach(content => content.classList.remove('active'));

    // 尝试按文字匹配按钮
    let activatedBtn = null;
    document.querySelectorAll('.tab-btn').forEach(btn => {
        const txt = (btn.textContent || '').trim();
        if ((tab === 'login' && /登录/.test(txt)) || (tab === 'register' && /注册/.test(txt))) {
            activatedBtn = btn;
        }
    });
    if (!activatedBtn) {
        const btns = document.querySelectorAll('.tab-btn');
        activatedBtn = (tab === 'login') ? btns[0] : (btns[1] || btns[0]);
    }
    if (activatedBtn) activatedBtn.classList.add('active');

    const targetForm = (tab === 'login') ? document.getElementById('login-form') : document.getElementById('register-form');
    if (targetForm) targetForm.classList.add('active');
}

// 显示消息（自动隐藏），支持类型 'info' | 'success' | 'error'
function showMessage(message, type = 'info') {
    const messageBox = document.getElementById('message-box');
    const messageText = document.getElementById('message-text');

    if (!messageBox || !messageText) {
        console.warn('message-box 或 message-text 元素未找到');
        return;
    }

    messageText.textContent = message;
    messageBox.className = 'message-box show';

    if (type === 'error') {
        messageBox.style.background = 'linear-gradient(135deg, #ff416c 0%, #ff4b2b 100%)';
    } else if (type === 'success') {
        messageBox.style.background = 'linear-gradient(135deg, #00b09b 0%, #96c93d 100%)';
    } else {
        messageBox.style.background = 'linear-gradient(135deg, #4a00e0 0%, #8e2de2 100%)';
    }

    clearTimeout(showMessage._timer);
    showMessage._timer = setTimeout(() => {
        messageBox.classList.remove('show');
    }, 3000);
}

// 验证数值范围
function judgment(value, min, max) {
    const num = parseFloat(value);
    if (isNaN(num) || num < min || num > max) {
        return false;
    }
    return true;
}

// 注册用户
function register() {
    console.debug('register() called');
    const idRaw = document.getElementById('reg-id').value.trim();
    const id = parseInt(idRaw, 10);
    const password = document.getElementById('reg-password').value.trim();
    const identity = document.getElementById('reg-identity').value;

    if (!idRaw || !password) {
        showMessage('请输入完整的注册信息', 'error');
        return;
    }

    if (users.length >= MAX_USERS) {
        showMessage('用户数量已达上限，无法注册', 'error');
        return;
    }

    if (isNaN(id)) {
        showMessage('用户名必须为数字学号格式', 'error');
        return;
    }

    if (users.some(u => u.user_id === id)) {
        showMessage('用户已存在，请更换用户名', 'error');
        return;
    }

    if (identity !== '教师' && identity !== '学生') {
        showMessage('身份请选择 "教师" 或 "学生"', 'error');
        return;
    }

    const newUser = {
        user_id: id,
        password: password,
        identity: identity
    };

    users.push(newUser);
    saveUsers();
    showMessage('注册成功，请登录', 'success');

    // 切换回登录标签并清空注册表单
    switchTab('login');
    document.getElementById('reg-id').value = '';
    document.getElementById('reg-password').value = '';
}

// 登录
function login() {
    const idRaw = document.getElementById('login-id').value.trim();
    const id = parseInt(idRaw, 10);
    const password = document.getElementById('login-password').value;
    const identity = document.getElementById('login-identity').value;

    if (isNaN(id)) {
        showMessage('请输入有效的用户名（数字）', 'error');
        return;
    }

    if (identity !== '学生' && identity !== '教师') {
        showMessage('身份请选择 "教师" 或 "学生"', 'error');
        return;
    }

    const user = users.find(u =>
        u.user_id === id &&
        u.password === password &&
        u.identity === identity
    );

    if (user) {
        currentUser = user;
        showMessage('登录成功', 'success');

        // 隐藏登录区，显示对应面板
        document.getElementById('login-section').classList.remove('active');
        if (user.identity === '教师') {
            const usernameSpan = document.getElementById('teacher-username');
            if (usernameSpan) usernameSpan.textContent = `教师 ${user.user_id}`;
            document.getElementById('teacher-section').classList.add('active');
            activateTeacherPanel('add');
            renderStudentsTable();
        } else {
            const usernameSpan = document.getElementById('student-username');
            if (usernameSpan) usernameSpan.textContent = `学生 ${user.user_id}`;
            document.getElementById('student-section').classList.add('active');
            activateStudentPanel('query');
            loadStudentInfo();
        }
    } else {
        showMessage('登录失败，用户名或密码或身份不正确', 'error');
    }
}

// 退出登录
function logout() {
    currentUser = null;
    document.querySelectorAll('.section').forEach(section => section.classList.remove('active'));
    document.getElementById('login-section').classList.add('active');
    showMessage('已退出系统', 'info');
}

// 激活教师侧栏某面板（el 可选）
function activateTeacherPanel(panelId, el = null) {
    document.querySelectorAll('#teacher-section .menu-btn').forEach(btn => btn.classList.remove('active'));
    document.querySelectorAll('#teacher-section .panel').forEach(panel => panel.classList.remove('active'));
    if (el) el.classList.add('active');
    else {
        const btn = document.querySelector(`#teacher-section .menu-btn[data-panel="${panelId}"]`);
        if (btn) btn.classList.add('active');
    }
    const panel = document.getElementById(`${panelId}-panel`);
    if (panel) panel.classList.add('active');
}

// 激活学生侧栏某面板（el 可选）
function activateStudentPanel(panelId, el = null) {
    document.querySelectorAll('#student-section .menu-btn').forEach(btn => btn.classList.remove('active'));
    document.querySelectorAll('#student-section .panel').forEach(panel => panel.classList.remove('active'));
    if (el) el.classList.add('active');
    else {
        const btn = document.querySelector(`#student-section .menu-btn[data-panel="${panelId}"]`);
        if (btn) btn.classList.add('active');
    }
    const panel = document.getElementById(panelId === 'query' ? 'student-query-panel' : 'myinfo-panel');
    if (panel) panel.classList.add('active');
}

// 供 index.html 按钮调用
function showTeacherPanel(panelId, el) { activateTeacherPanel(panelId, el); }
function showStudentPanel(panelId, el) { activateStudentPanel(panelId, el); }

// 教师：添加学生
function addStudent() {
    const idRaw = document.getElementById('add-id').value.trim();
    const id = parseInt(idRaw, 10);
    const name = document.getElementById('add-name').value.trim();
    const ageRaw = document.getElementById('add-age').value.trim();
    const age = parseInt(ageRaw, 10);
    const grade = document.getElementById('add-grade').value.trim();
    const profession = document.getElementById('add-profession').value.trim();
    const mathRaw = document.getElementById('add-math').value.trim();
    const englishRaw = document.getElementById('add-english').value.trim();
    const programmingRaw = document.getElementById('add-programming').value.trim();

    const math = parseFloat(mathRaw);
    const english = parseFloat(englishRaw);
    const programming = parseFloat(programmingRaw);

    if (!idRaw || !name || isNaN(age) || !grade || !profession ||
        isNaN(math) || isNaN(english) || isNaN(programming)) {
        showMessage('请输入完整的学生信息', 'error');
        return;
    }

    if (!judgment(age, 1, 150) || !judgment(math, 0, 100) ||
        !judgment(english, 0, 100) || !judgment(programming, 0, 100)) {
        showMessage('年龄或成绩不在有效范围内', 'error');
        return;
    }

    if (students.some(s => s.id === id)) {
        showMessage('该学号的学生已存在', 'error');
        return;
    }

    const newStudent = {
        id: id,
        name: name,
        age: age,
        grade: grade,
        profession: profession,
        Math: math,
        English: english,
        Programming: programming
    };

    students.push(newStudent);
    saveStudents();
    renderStudentsTable();
    showMessage('添加学生成功', 'success');

    // 清空输入
    document.querySelectorAll('#add-panel input').forEach(input => input.value = '');
}

// 教师：通过删除面板删除学生
function deleteStudent() {
    const idRaw = document.getElementById('delete-id').value.trim();
    const id = parseInt(idRaw, 10);

    if (isNaN(id)) {
        showMessage('请输入要删除的学号', 'error');
        return;
    }

    const index = students.findIndex(s => s.id === id);
    if (index !== -1) {
        students.splice(index, 1);
        saveStudents();
        renderStudentsTable();
        showMessage('删除学生成功', 'success');
        document.getElementById('delete-id').value = '';
    } else {
        showMessage('未找到该学生信息', 'error');
    }
}

// 教师：修改学生成绩（面板）
function modifyStudent() {
    const idRaw = document.getElementById('modify-id').value.trim();
    const id = parseInt(idRaw, 10);
    const math = parseFloat(document.getElementById('modify-math').value);
    const english = parseFloat(document.getElementById('modify-english').value);
    const programming = parseFloat(document.getElementById('modify-programming').value);

    if (isNaN(id) || isNaN(math) || isNaN(english) || isNaN(programming)) {
        showMessage('请输入要修改的完整信息', 'error');
        return;
    }

    if (!judgment(math, 0, 100) || !judgment(english, 0, 100) || !judgment(programming, 0, 100)) {
        showMessage('成绩需在 0-100 范围内', 'error');
        return;
    }

    const student = students.find(s => s.id === id);
    if (student) {
        student.Math = math;
        student.English = english;
        student.Programming = programming;
        saveStudents();
        renderStudentsTable();
        showMessage('修改成绩成功', 'success');
        document.querySelectorAll('#modify-panel input').forEach(input => input.value = '');
    } else {
        showMessage('未找到该学生信息', 'error');
    }
}

// 教师：查询学生（按学号或姓名）
function queryStudent() {
    const queryType = document.querySelector('input[name="query-type"]:checked').value;
    const queryValue = document.getElementById('query-value').value.trim();
    const resultContainer = document.getElementById('query-result');

    if (!queryValue) {
        showMessage('请输入查询值', 'error');
        return;
    }

    let result;
    if (queryType === 'id') {
        const id = parseInt(queryValue, 10);
        if (isNaN(id)) { showMessage('请输入有效学号', 'error'); return; }
        result = students.find(s => s.id === id);
    } else {
        result = students.find(s => s.name === queryValue);
    }

    if (result) {
        resultContainer.innerHTML = `
            <div class="student-info">
                <h3>查询结果</h3>
                <div class="info-grid">
                    <div><strong>学号：</strong>${result.id}</div>
                    <div><strong>姓名：</strong>${result.name}</div>
                    <div><strong>年龄：</strong>${result.age}</div>
                    <div><strong>年级：</strong>${result.grade}</div>
                    <div><strong>专业：</strong>${result.profession}</div>
                    <div><strong>数学：</strong>${result.Math}</div>
                    <div><strong>英语：</strong>${result.English}</div>
                    <div><strong>程序设计：</strong>${result.Programming}</div>
                    <div><strong>平均分：</strong>${((result.Math + result.English + result.Programming) / 3).toFixed(2)}</div>
                    <div><strong>总分：</strong>${(result.Math + result.English + result.Programming).toFixed(2)}</div>
                </div>
            </div>
        `;
    } else {
        resultContainer.innerHTML = '<p class="no-result">未找到学生信息</p>';
    }
}

// 学生端：查询（与教师查询类似，但显示项稍有不同）
function studentQuery() {
    const queryType = document.querySelector('input[name="student-query-type"]:checked').value;
    const queryValue = document.getElementById('student-query-value').value.trim();
    const resultContainer = document.getElementById('student-query-result');

    if (!queryValue) {
        showMessage('请输入查询值', 'error');
        return;
    }

    let result;
    if (queryType === 'id') {
        const id = parseInt(queryValue, 10);
        if (isNaN(id)) { showMessage('请输入有效学号', 'error'); return; }
        result = students.find(s => s.id === id);
    } else {
        result = students.find(s => s.name === queryValue);
    }

    if (result) {
        resultContainer.innerHTML = `
            <div class="student-info">
                <h3>查询结果</h3>
                <div class="info-grid">
                    <div><strong>学号：</strong>${result.id}</div>
                    <div><strong>姓名：</strong>${result.name}</div>
                    <div><strong>年龄：</strong>${result.age}</div>
                    <div><strong>年级：</strong>${result.grade}</div>
                    <div><strong>专业：</strong>${result.profession}</div>
                    <div><strong>数学：</strong>${result.Math}</div>
                    <div><strong>英语：</strong>${result.English}</div>
                    <div><strong>程序设计：</strong>${result.Programming}</div>
                </div>
            </div>
        `;
    } else {
        resultContainer.innerHTML = '<p class="no-result">未找到学生信息</p>';
    }
}

// 学生端：加载当前登录学生信息
function loadStudentInfo() {
    const infoContainer = document.getElementById('my-info-container');
    if (!currentUser) return;

    const student = students.find(s => s.id === currentUser.user_id);
    if (student) {
        const average = ((student.Math + student.English + student.Programming) / 3).toFixed(2);
        const total = (student.Math + student.English + student.Programming).toFixed(2);

        infoContainer.innerHTML = `
            <div class="my-info">
                <h3>个人信息</h3>
                <div class="info-grid">
                    <div><strong>学号：</strong>${student.id}</div>
                    <div><strong>姓名：</strong>${student.name}</div>
                    <div><strong>年龄：</strong>${student.age}岁</div>
                    <div><strong>年级：</strong>${student.grade}</div>
                    <div><strong>专业：</strong>${student.profession}</div>
                </div>

                <h3 style="margin-top: 30px;">成绩信息</h3>
                <div class="scores">
                    <div class="score-item">
                        <div class="score-label">数学</div>
                        <div class="score-value ${student.Math >= 60 ? 'pass' : 'fail'}">${student.Math}</div>
                    </div>
                    <div class="score-item">
                        <div class="score-label">英语</div>
                        <div class="score-value ${student.English >= 60 ? 'pass' : 'fail'}">${student.English}</div>
                    </div>
                    <div class="score-item">
                        <div class="score-label">程序设计</div>
                        <div class="score-value ${student.Programming >= 60 ? 'pass' : 'fail'}">${student.Programming}</div>
                    </div>
                </div>

                <div class="summary">
                    <div><strong>平均分：</strong>${average}</div>
                    <div><strong>总分：</strong>${total}</div>
                </div>
            </div>
        `;
    } else {
        infoContainer.innerHTML = '<p class="no-data">未找到对应的学生信息，请联系教师录入</p>';
    }
}

// 渲染全部学生表格
function renderStudentsTable() {
    const tbody = document.getElementById('students-tbody');
    if (!tbody) return;

    if (students.length === 0) {
        tbody.innerHTML = '<tr><td colspan="9" class="no-data">暂无学生数据</td></tr>';
        return;
    }

    let html = '';
    students.forEach(student => {
        html += `
            <tr>
                <td>${student.id}</td>
                <td>${student.name}</td>
                <td>${student.age}</td>
                <td>${student.grade}</td>
                <td>${student.profession}</td>
                <td class="${student.Math >= 60 ? 'pass' : 'fail'}">${student.Math}</td>
                <td class="${student.English >= 60 ? 'pass' : 'fail'}">${student.English}</td>
                <td class="${student.Programming >= 60 ? 'pass' : 'fail'}">${student.Programming}</td>
                <td>
                    <button class="btn-action" onclick="editStudent(${student.id})"><i class="fas fa-edit"></i></button>
                    <button class="btn-action btn-delete" onclick="quickDelete(${student.id})"><i class="fas fa-trash"></i></button>
                </td>
            </tr>
        `;
    });

    tbody.innerHTML = html;
}

// 表格中的快速删除（带确认）
function quickDelete(id) {
    if (!confirm('确认要删除该学生信息吗？')) return;
    const index = students.findIndex(s => s.id === id);
    if (index !== -1) {
        students.splice(index, 1);
        saveStudents();
        renderStudentsTable();
        showMessage('删除成功', 'success');
    } else {
        showMessage('未找到该学生', 'error');
    }
}

// 表格中编辑按钮：切换到修改面板并填入数据
function editStudent(id) {
    const student = students.find(s => s.id === id);
    if (!student) { showMessage('未找到该学生', 'error'); return; }
    const btn = document.querySelector(`#teacher-section .menu-btn[data-panel="modify"]`);
    activateTeacherPanel('modify', btn);
    document.getElementById('modify-id').value = student.id;
    document.getElementById('modify-math').value = student.Math;
    document.getElementById('modify-english').value = student.English;
    document.getElementById('modify-programming').value = student.Programming;
}

// 保存 users 与 students 到 localStorage（以及提供单独接口）
function saveData() {
    saveUsers();
    saveStudents();
    showMessage('数据保存成功', 'success');
}

function saveUsers() {
    localStorage.setItem('users', JSON.stringify(users));
}

function saveStudents() {
    localStorage.setItem('students', JSON.stringify(students));
}

// 从 localStorage 加载（并处理可能的解析错误）
function loadFromLocalStorage() {
    const savedUsers = localStorage.getItem('users');
    const savedStudents = localStorage.getItem('students');

    if (savedUsers) {
        try {
            users = JSON.parse(savedUsers);
        } catch (e) {
            console.error('解析 users 失败：', e);
            // 保持原有默认 users（避免覆盖）
        }
    }

    if (savedStudents) {
        try {
            students = JSON.parse(savedStudents);
        } catch (e) {
            console.error('解析 students 失败：', e);
            // 保持原有默认 students（避免覆盖）
        }
    }
}

// 安全注入样式（替代多行模板字面量 insertAdjacentHTML，避免编码/转义问题）
(function injectSafeStyles() {
    const css = [
        ".no-data { text-align: center; padding: 20px; color: #666; font-style: italic; }",
        ".student-info { padding: 20px; }",
        ".info-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-top: 15px; }",
        ".info-grid div { padding: 10px; background: #f8f9fa; border-radius: 8px; }",
        ".btn-action { padding: 8px 12px; margin: 0 5px; border: none; border-radius: 6px; cursor: pointer; background: #4a00e0; color: white; transition: all 0.3s; }",
        ".btn-action:hover { opacity: 0.8; transform: scale(1.05); }",
        ".btn-delete { background: #ff416c; }",
        ".pass { color: #00b09b; font-weight: bold; }",
        ".fail { color: #ff416c; font-weight: bold; }",
        ".scores { display:flex; gap:20px; margin:20px 0; }",
        ".score-item { flex:1; text-align:center; }",
        ".score-label { font-weight:bold; margin-bottom:5px; }",
        ".score-value { font-size:2rem; font-weight:bold; padding:10px; border-radius:10px; background:#f8f9fa; }",
        ".score-value.pass { color:#00b09b; }",
        ".score-value.fail { color:#ff416c; }",
        ".summary { display:flex; justify-content:space-between; margin-top:20px; padding-top:20px; border-top:2px solid #eee; }"
    ].join("\n");

    const styleEl = document.createElement("style");
    styleEl.type = "text/css";
    styleEl.textContent = css;
    document.head.appendChild(styleEl);
})();