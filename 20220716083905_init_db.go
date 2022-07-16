package main

import (
	"context"

	"github.com/uptrace/bun"
)

func init() {
	Migrations.MustRegister(func(ctx context.Context, db *bun.DB) error {
		db.NewCreateTable().Model(&databaseServer{}).Exec(ctx)
		return nil
	}, func(ctx context.Context, db *bun.DB) error {
		db.NewDropTable().Model(&databaseServer{}).Exec(ctx)
		return nil
	})
}
