#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <libcloud/cloud.hpp>
#include <libcore/core.hpp>
#include <libstorage/storage.hpp>
#include <libutil/util.hpp>

int main([[maybe_unused]] int argc, char* argv[]) {
  USE_NAMESPACE

  if (argc != 2) {
    std::cerr << "should pass 1 parameter either destroy or create"
              << std::endl;
    return ATM_ERR;
  }

  if (initialize_core()) {
    std::cerr << "cannot initialize config, state, and logger!" << std::endl;
    return ATM_ERR;
  }

  // config
  auto*         config = Config::get();
  cloud::Config cloud_config{config};

  auto cloud_db = std::make_unique<cloud::Database>(&cloud_config);
  auto conn = cloud_db->connection();

  if (strcmp(argv[1], "destroy") == 0) {
    // cloud_db->remove("TRAY_123");
    conn->execute("DROP TABLE IF EXISTS images");
    conn->execute("DROP TYPE SKU_CARD_T");
    conn->execute("DROP TYPE SKU_NUMBER_T");
    conn->execute("DROP TYPE INFECTION_T");
    LOG_INFO("Successfully delete `images` table");
  } else if (strcmp(argv[1], "create") == 0) {
    conn->execute(
        "CREATE TYPE SKU_CARD_T AS ENUM ("
        "'Colonize', "
        "'TEND', "
        "'ExposeFlip', "
        "'FlipNylonFlip', "
        "'TendFlipFlipCompFlip', "
        "'FlipTend', "
        "'HarvestBed', "
        "'NOT_USED'"
        ");");

    conn->execute(
        "CREATE TYPE SKU_NUMBER_T AS ENUM ("
        "'REGEN', "
        "'ESKIMO', "
        "'TIGER', "
        "'SAPIEN', "
        "'NOT_USED'"
        ");");

    conn->execute(
        "CREATE TYPE INFECTION_T AS ENUM ("
        "'NONE', "
        "'BLUE/GREEN', "
        "'BLACK/BROWN', "
        "'METABOLITE', "
        "'NOT_USED'"
        ");");

    conn->execute(
        "CREATE TABLE images( "
        "ID  SERIAL PRIMARY KEY, "
        "HASH TEXT NOT NULL UNIQUE, "
        "YEAR INTEGER NOT NULL, "
        "MONTH INTEGER NOT NULL, "
        "DAY INTEGER NOT NULL, "
        "HOUR INTEGER NOT NULL, "
        "MINUTE INTEGER NOT NULL, "
        "SECOND INTEGER NOT NULL, "
        "SKU_CARD SKU_CARD_T NOT NULL, "
        "SKU_NUMBER SKU_NUMBER_T NOT NULL, "
        "SKU_PROD_DAY INTEGER NOT NULL, "
        "TRAY_BARCODE INTEGER NOT NULL, "
        "LID_BARCODE TEXT NOT NULL, "
        "BATCH_ID INTEGER NOT NULL, "
        "INFECTION INFECTION_T NOT NULL"
        ");");

    conn->execute("CREATE UNIQUE INDEX HASH_IDX ON images (HASH)");
    conn->execute(
        "CREATE UNIQUE INDEX HASH_LOWER_IDX ON images ((lower(HASH)))");

    cloud_db->prepare_statements();
    // cloud_db->insert({-1, "TRAY_123", 2020, 10, 13, 0, 0, 0, "Colonize",
    //                   "REGEN", 1, 123123, "NO_LID:12345", 2, "METABOLITE"});
    // auto img = cloud_db->get("TRAY_123");
    // LOG_INFO("{}", img);
    LOG_INFO("Successfully create `images` table");
  } else {
    LOG_INFO("Parameter should be only destroy or create");
  }
}
