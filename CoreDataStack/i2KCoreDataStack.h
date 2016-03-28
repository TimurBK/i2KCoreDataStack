//
//  i2KCoreDataStack.h
//  i2KCoreDataStack
//
//  Created by Timur Kuchkarov on 28.02.16.
//  Copyright © 2016 Timur Kuchkarov. All rights reserved.
//

@import Foundation;
@import CoreData;

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, i2KCDPStoreType) {
	i2KCDPStoreTypeSQLite = 0,
	i2KCDPStoreTypeInMemory = 1
};

@interface i2KCoreDataStack : NSObject

/**
 *  Managed object context tied to main queue
 */
@property (nonatomic, strong, readonly) NSManagedObjectContext *mainManagedObjectContext;

/**
 *  Private queue based MOC with same persistent store coordinator as main MOC.
 *  May be useful for preparing data for UI, either fetching objectIDs and passing them to mainMOC 
 *  or by fetching dictionaries.
 */
@property (nonatomic, strong, readonly) NSManagedObjectContext *dataExportManagedObjectContext;

/**
 *  Private queue based MOC with separate persistent store coordinator, useful for import large amounts of data
 */
@property (nonatomic, strong, readonly) NSManagedObjectContext *importManagedObjectContext;

/**
 *  Designated initializer for stack
 *
 *  @param storeType Store type, currently only SQLite and inmemory are supported
 *  @param storeURL  URL to store
 *  @param modelURL  URL to managed object model file
 *  @param options   options for PSC
 *
 *  @return initialized stack
 */
- (instancetype)initWithStoreType:(i2KCDPStoreType)storeType storeURL:(NSURL * _Nullable)storeURL modelURL:(NSURL *)modelURL options:(NSDictionary *)options NS_DESIGNATED_INITIALIZER;

/**
 *  Shorthand for creating automigrated stack with SQLite storage
 *
 *  @param modelName name of model
 *
 *  @return initialized stack
 */
- (instancetype)initWithAutoMigratingSQLiteStoreWithModelName:(NSString *)modelName;

/**
 *  Shorthand for creating inmemory stack
 *
 *  @param modelName name of model
 *
 *  @return initialized stack
 */
- (instancetype)initWithAutoMigratingInMemoryStoreWithModelName:(NSString *)modelName;

- (NSManagedObjectContext *)contextWithParent:(NSManagedObjectContext * _Nullable)parentContext concurrencyType:(NSManagedObjectContextConcurrencyType)concurrencyType persistentStoreCoordinator:(NSPersistentStoreCoordinator *)coordinator;
- (NSManagedObjectContext *)createPrivateQueueContext;
- (NSManagedObjectContext *)createMainQueueContext;
- (NSManagedObjectContext *)createBackgroundImportContext;

@end

NS_ASSUME_NONNULL_END
