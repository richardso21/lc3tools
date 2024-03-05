const state = {
  theme: "light",
  number_type: "unsigned",
  editor_binding: "standard",
  ignore_privilege: false,
  liberal_asm: false,
  ignore_update: false,
  run_until_halt: false,
  clear_out_on_reload: true,
	autocomplete: 'full' // 'none' | 'basic' | 'full'
};

const mutations = {
  setTheme(state, theme) {
    state.theme = theme;
  },
  setNumberType(state, setting) {
    state.number_type = setting;
  },
  setEditorBinding(state, setting) {
    state.editor_binding = setting;
  },
  setIgnorePrivilege(state, setting) {
    state.ignore_privilege = setting;
  },
  setLiberalAsm(state, setting) {
    state.liberal_asm = setting;
  },
  setIgnoreUpdate(state, setting) {
    state.ignore_update = setting;
  },
  setRunUntilHalt(state, setting) {
    state.run_until_halt = setting;
  },
  setClearOutOnReload(state, setting) {
    state.clear_out_on_reload = setting;
  },
  setAutocomplete(state, setting) {
    state.autocomplete = setting;
  },
};

const getters = {
  theme: (state) => state.theme,
  number_type: (state) => state.number_type,
  editor_binding: (state) => state.editor_binding,
  ignore_privilege: (state) => state.ignore_privilege,
  liberal_asm: (state) => state.liberal_asm,
  ignore_update: (state) => state.ignore_update,
  run_until_halt: (state) => state.run_until_halt,
  clear_out_on_reload: (state) => state.clear_out_on_reload,
  autocomplete: (state) => state.autocomplete,
};

export default {
  state,
  mutations,
  getters,
};
