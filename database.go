package main

import (
	"context"
	"database/sql"
	"fmt"
	"time"

	"github.com/uptrace/bun"
	"github.com/uptrace/bun/dialect/pgdialect"
	"github.com/uptrace/bun/driver/pgdriver"
	"github.com/uptrace/bun/migrate"
)

var db *bun.DB

func setupDatabase() error {
	ctx := context.Background()
	connectDatabase()
	migrator := migrate.NewMigrator(db, Migrations)
	err := migrator.Init(ctx)
	if err != nil {
		return err
	}
	_, err = migrator.Migrate(ctx)
	if err != nil {
		return err
	}
	err = db.Close()
	if err != nil {
		return err
	}
	return nil
}

func connectDatabase() {
	sqldb := sql.OpenDB(pgdriver.NewConnector(pgdriver.WithDSN(buildConnectionString())))

	db = bun.NewDB(sqldb, pgdialect.New())

	// TODO: How to make this extend to life of daemon and still be closed properly
	// defer db.Close()

	db.SetConnMaxLifetime(time.Minute * 3)
	db.SetMaxOpenConns(10)
	db.SetMaxIdleConns(10)
}

func getSingleServerData(ctx context.Context, id int64) (*databaseServer, error) {
	server := new(databaseServer)
	exists, err := db.NewSelect().Model(server).Where("id = ?", id).Exists(ctx)
	if err != nil {
		return nil, err
	}
	if !exists {
		return nil, nil
	}
	err = db.NewSelect().Model(server).Where("id = ?", id).Scan(ctx)
	if err != nil {
		return nil, err
	}

	return server, nil
}

func addServerToDatabase(ctx context.Context, server requestServer) error {
	_, err := db.NewInsert().Model(convertRequestServerToDBServer(server)).Exec(ctx)
	if err != nil {
		return err
	}
	return nil
}

func collectServerData(ctx context.Context) ([]responseServer, error) {
	var servers []databaseServer
	err := db.NewSelect().Model(&servers).Scan(ctx)
	if err != nil {
		return nil, err
	}

	var serverList []responseServer
	for _, dbServer := range servers {
		server := responseServer{
			dbServer.ID,
			dbServer.Name,
			dbServer.Directory,
			dbServer.JarFile,
			dbServer.RunMemory,
			dbServer.StartMemory,
			dbServer.JavaArgs,
			dbServer.Restart,
			getServerStatus(dbServer.ID),
		}
		serverList = append(serverList, server)
	}

	return serverList, nil
}

// A duplicate server is defined by a server having either the same name or directory as another.
func getServerByDirectory(ctx context.Context, directory string) (*databaseServer, error) {
	server := new(databaseServer)
	exists, err := db.NewSelect().Model(server).Where("directory = ?", directory).Exists(ctx)
	if err != nil {
		return nil, err
	}
	if !exists {
		return nil, nil
	}
	err = db.NewSelect().Model(server).Where("directory = ?", directory).Scan(ctx)
	if err != nil {
		return nil, err
	}

	return server, nil
}

func modifyServerEntry(ctx context.Context, server requestServer, serverID int64) error {
	_, err := db.NewUpdate().Model(convertRequestServerToDBServerWithID(server, serverID)).WherePK().Exec(ctx)
	if err != nil {
		return err
	}

	return nil
}

func getServerStatus(serverID int64) ServerStatus {
	server := runningServers.Find(serverID)
	if server != nil {
		return Running
	} else {
		return Stopped
	}
}

func removeServerFromDB(ctx context.Context, serverID int64) error {
	_, err := db.NewDelete().Model(&databaseServer{}).Where("id = ?", serverID).Exec(ctx)
	if err != nil {
		return err
	}
	return nil
}

// Helpers

func convertRequestServerToDBServer(server requestServer) *databaseServer {
	return &databaseServer{
		Name:        server.Name,
		Directory:   server.Directory,
		JarFile:     server.JarFile,
		RunMemory:   server.RunMemory,
		StartMemory: server.StartMemory,
		JavaArgs:    server.JavaArgs,
	}
}

func convertRequestServerToDBServerWithID(server requestServer, id int64) *databaseServer {
	return &databaseServer{
		ID:          id,
		Name:        server.Name,
		Directory:   server.Directory,
		JarFile:     server.JarFile,
		RunMemory:   server.RunMemory,
		StartMemory: server.StartMemory,
		JavaArgs:    server.JavaArgs,
	}
}

func buildConnectionString() string {
	return "postgres://" + config.Database.User + ":" + config.Database.Password +
		"@" + config.Database.Host + ":" + fmt.Sprint(config.Database.Port) +
		"/" + config.Database.Database + "?sslmode=disable"
}
