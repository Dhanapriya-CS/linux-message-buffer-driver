# Linux Kernel Lab Tracker

A standalone portfolio web app inspired by the structure of an IT onboarding tracker. It tracks Linux kernel learning projects across build, test, and review stages.

## Features

- Summary cards for total, completed, in-progress, and blocked projects.
- Add, edit, and remove project records.
- Search by project details and filter by overall progress.
- Export records to CSV.
- Responsive layout for desktop and mobile.
- Fictional sample records; data stays in browser `localStorage`.

## Preview locally

Open `index.html` in a browser. No installation or build step is required.

## Publish with GitHub Pages

1. Create a new public GitHub repository, for example `linux-kernel-lab-tracker`.
2. Upload `index.html` and `README.md` to the repository root.
3. In repository settings, open **Pages** and select the `main` branch and `/ (root)` folder.
4. Wait for the Pages deployment, then use the published URL.

## Notes

This is a front-end tracker only. It does not build a kernel, load modules, or execute tests. Browser storage is local to the current browser profile. Use fictional or non-sensitive information.
