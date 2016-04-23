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
 *  Default persistent store coordinator
 */
@property (nonatomic, strong, readonly) NSPersistentStoreCoordinator *defaultCoordinator;

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

/**
 *  Creates context with specified parameters. If you provide parent context, make really sure you need it as parent-child setup may hurt performance.
 *
 *  @param parentContext   Context to be set as parent context for returning one.
 *  @param concurrencyType Concurrency type of context (NSPrivateQueueConcurrencyType or NSMainQueueConcurrencyType).
 *  @param coordinator     Instance of NSPersistentStoreCoordinator for context to connect to.
 *
 *  @return Instance of NSManagedObjectContext with specified parammeters.
 */
- (NSManagedObjectContext *)contextWithParent:(NSManagedObjectContext * _Nullable)parentContext concurrencyType:(NSManagedObjectContextConcurrencyType)concurrencyType persistentStoreCoordinator:(NSPersistentStoreCoordinator *)coordinator;

/**
 *  Creates private queue context directly connected to default persistent store coordinator.
 *
 *  @return Instance of NSManagedObjectContext with NSPrivateQueueConcurrencyType directly connected to default NSPersistentStoreCoordinator.
 */
- (NSManagedObjectContext *)createPrivateQueueContextWithDefaultCoordinator;

/**
 *  Creates main queue context directly connected to default persistent store coordinator.
 *
 *  @return Instance of NSManagedObjectContext with NSMainQueueConcurrencyType directly connected to default NSPersistentStoreCoordinator.
 */
- (NSManagedObjectContext *)createMainQueueContextWithDefaultCoordinator;

/**
 *  Creates private queue context directly connected to persistent store coordinator. Since persistent store coordinator
 *	is not the same as default one, it may be useful for large import operations as it won't block coordinator of
 *	default main queue context.
 *
 *  @return Instance of NSManagedObjectContext with NSPrivateQueueConcurrencyType directly connected to instance of NSPersistentStoreCoordinator.
 */
- (NSManagedObjectContext *)createBackgroundImportContext;

/**
 *  Creates private queue context with parent context. Make sure you really need parent-child context setup as it may hurt performance.
 *
 *  @param parentContext Context to be set as parent context for returning one.
 *
 *  @return Instance of NSManagedObjectContext with NSMainQueueConcurrencyType with parent context set.
 */
- (NSManagedObjectContext *)createPrivateQueueContextWithParentContext:(NSManagedObjectContext *)parentContext;

/**
 *  Creates main queue context with parent context. Make sure you really need parent-child context setup as it may hurt performance.
 *
 *  @param parentContext Context to be set as parent context for returning one.
 *
 *  @return Instance of NSManagedObjectContext with NSMainQueueConcurrencyType with parent context set.
 */
- (NSManagedObjectContext *)createMainQueueContextWithParentContext:(NSManagedObjectContext *)parentContext;

/**
 *  Creates private queue context directly connected to provided persistent store coordinator.
 *
 *  @param coordinator Instance of NSPersistentStoreCoordinator for context to connect to.
 *
 *  @return Instance of NSManagedObjectContext with NSPrivateQueueConcurrencyType directly connected to provided NSPersistentStoreCoordinator.
 */
- (NSManagedObjectContext *)createPrivateQueueContextWithCoordinator:(NSPersistentStoreCoordinator *)coordinator;

/**
 *  Creates main queue context directly connected to provided persistent store coordinator.
 *
 *  @param coordinator Instance of NSPersistentStoreCoordinator for context to connect to.
 *
 *  @return Instance of NSManagedObjectContext with NSMainQueueConcurrencyType directly connected to provided NSPersistentStoreCoordinator.
 */
- (NSManagedObjectContext *)createMainQueueContextWithCoordinator:(NSPersistentStoreCoordinator *)coordinator;

@end

NS_ASSUME_NONNULL_END
