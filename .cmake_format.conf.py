with section("format"):

  line_width = 80
  tab_size = 2
  use_tabchars = False

with section("markup"):
  enable_markup = False

additional_commands = {
  "FetchContent_Declare": {
    "pargs": 1,
    "flags": [],
    "kwargs": {
      "URL": '1',
      "URL_HASH": '1',
      "GIT_REPOSITORY": '1',
      "GIT_PROGRESS": '1',
      "GIT_TAG": '1',
      "BINARY_DIR": '1',
      "SOURCE_DIR": '1',
      "DOWNLOAD_DIR": '1',
      "SUBBUILD_DIR": "1",
      "UPDATE_DISCONNECTED": "1",
    }
  }
}
